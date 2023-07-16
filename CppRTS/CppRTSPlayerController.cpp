// Copyright Epic Games, Inc. All Rights Reserved.

#include "CppRTSPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "CppRTSCharacter.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ACppRTSPlayerController::ACppRTSPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;

	//Behind-The-Scenes
	bShift = false;
}

void ACppRTSPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void ACppRTSPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ACppRTSPlayerController::OnSetDestinationStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &ACppRTSPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ACppRTSPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &ACppRTSPlayerController::OnSetDestinationReleased);
	}
}

void ACppRTSPlayerController::OnSetDestinationStarted()
{
	FHitResult Hit;
	bool bHitSuccessful = false;

	bHitSuccessful = APlayerController::GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	if (bHitSuccessful) {
		AssignMoveTargets(Selects, Hit.Location);
		for (int i=0;i<Selects.Num();i++) {
			Selects[i]->AddDestination(Hit.Location, bShift, false);
		}
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FXCursor, Hit.Location);
	}
}

void ACppRTSPlayerController::AssignMoveTargets(TArray<ACppRTSCharacter*> Units, FVector ClickLocation) {
	TArray<ACppRTSCharacter*> NearUnits;
	TArray<ACppRTSCharacter*> FarUnits;

	TArray<AActor*> UnitActors;
	
	for (int i=0;i<Units.Num();i++) {
		NearUnits.Add(Units[i]);
		UnitActors.Add(Cast<AActor>(Units[i]));
	}
	FVector AvgLoc = UGameplayStatics::GetActorArrayAverageLocation(UnitActors);

	float DistSqSum = 0.;
	for (int i=0;i<Units.Num();i++) {
		DistSqSum += FVector::DistSquared(Units[i]->GetActorLocation(), AvgLoc);
	}

	float stdev = sqrt(DistSqSum/Units.Num());

	if (stdev < 400.) {
		int whileInc = 0;
		while (whileInc < NearUnits.Num()) {
			if (FVector::Dist(NearUnits[whileInc]->GetActorLocation(), AvgLoc) > stdev) {
				FarUnits.Add(NearUnits[whileInc]);
				NearUnits.RemoveAt(whileInc);
			}
			else {	whileInc++;	}
		}

		for (int i=0;i<FarUnits.Num();i++) {
			FVector unitLoc = FarUnits[i]->GetActorLocation();
			FVector dir = FVector(unitLoc.X, unitLoc.Y, ClickLocation.Z) - ClickLocation;
			dir.Normalize(0.0001);
			FarUnits[i]->AddDestination(ClickLocation + (dir*stdev*0.25), bShift, true);
		}

		UnitActors.Empty();
		for (int i=0;i<NearUnits.Num();i++) {
			UnitActors.Add(Cast<AActor>(NearUnits[i]));
		}
		AvgLoc = UGameplayStatics::GetActorArrayAverageLocation(UnitActors);
		for (int i=0;i<NearUnits.Num();i++) {
			NearUnits[i]->AddDestination((NearUnits[i]->GetActorLocation() - AvgLoc) + ClickLocation, bShift, true);
		}
	}

	else {
		for (int i=0;i<Units.Num();i++) {
			FVector unitLoc = Units[i]->GetActorLocation();
			FVector dir = FVector(unitLoc.X, unitLoc.Y, ClickLocation.Z) - ClickLocation;
			dir.Normalize(0.0001);
			Units[i]->AddDestination(ClickLocation + (dir*50.), bShift, true);
		}
	}
}

// Triggered every frame when the input is held down
void ACppRTSPlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void ACppRTSPlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// Print test
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, TEXT("Hello!"));
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}
