// Copyright Gedalya Gordon and Epic Games, Inc. All Rights Reserved.

#include "CppRTSPlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/HUD.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "CppRTSCharacter.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

/* Engine */
ACppRTSPlayerController::ACppRTSPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	//Behind-The-Scenes
	bShift = false;
}

void ACppRTSPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		/* Set up mouse input events */
		EnhancedInputComponent->BindAction(RMBAction, ETriggerEvent::Started, this, &ACppRTSPlayerController::OnRMBDown); // RMB
		// LMB
		EnhancedInputComponent->BindAction(LMBAction, ETriggerEvent::Started, this, &ACppRTSPlayerController::OnLMBStarted);
		EnhancedInputComponent->BindAction(LMBAction, ETriggerEvent::Completed, this, &ACppRTSPlayerController::OnLMBReleased);
		EnhancedInputComponent->BindAction(LMBAction, ETriggerEvent::Canceled, this, &ACppRTSPlayerController::OnLMBReleased);
		// Shift
		EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &ACppRTSPlayerController::OnShiftDown);
		EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &ACppRTSPlayerController::OnShiftUp);
		EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Canceled, this, &ACppRTSPlayerController::OnShiftUp);
	}
}

/* Input */
void ACppRTSPlayerController::OnRMBDown() {
	FHitResult Hit;
	bool bHitSuccessful = false;
	
	bHitSuccessful = APlayerController::GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	if (bHitSuccessful) {
		AssignMoveTargets(Selects, Hit.Location);
		for (int i=0;i<Selects.Num();i++) {
			Selects[i]->AddDestination(Hit.Location, bShift, false);
		}
		if (Selects.Num() > 0) {
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FXCursor, Hit.Location);
		}
	}
}

void ACppRTSPlayerController::OnShiftUp() {
	bShift = false;
}

void ACppRTSPlayerController::OnShiftDown() {
	bShift = true;
}

void ACppRTSPlayerController::OnLMBStarted() {
	HUD_ref->BeginSelect();
}

void ACppRTSPlayerController::OnLMBReleased() {
	HUD_ref->EndSelect();
}

/* BeginPlay and Tick */
void ACppRTSPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Input Setup
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);

	/* Create references */

	// HUD
	HUD_ref = Cast<ACppRTSHUD>(GetHUD());
	if (HUD_ref == nullptr) {
		ConsoleCommand("EXIT");
	}
	HUD_ref->Controller_ref = this;
}

void ACppRTSPlayerController::Tick(float DeltaSeconds)
{
	// Call the base class
    Super::Tick(DeltaSeconds);

}

/* Methods */

// Selection
void ACppRTSPlayerController::Select(ACppRTSCharacter *Unit) {
	Selects.AddUnique(Unit);
	//Unit.ReceiveSelect(this, true)
}
void ACppRTSPlayerController::Deselect(ACppRTSCharacter *Unit) {
	Selects.Remove(Unit);
	//Unit.ReceiveSelect(this, false)
}
void ACppRTSPlayerController::ClearSelection() {
	for (int i=0;i<Selects.Num();i++) {
		//Selects[i].ReceiveSelect(this, false)
	}
	Selects.Empty();
}

// AI
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