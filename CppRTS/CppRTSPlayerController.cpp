// Copyright Gedalya Gordon and Epic Games, Inc. All Rights Reserved.

#include "CppRTSPlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/HUD.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "CppRTSCharacter.h"
#include "RTSUtils.h"
#include "AIController.h"
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

ACppRTSPlayerController::ACppRTSPlayerController(FName OwnershipTag) {
	OwnedTag = OwnershipTag;
	ACppRTSPlayerController();
}
ACppRTSPlayerController::ACppRTSPlayerController(bool bAutoAttackNeutrals) {
	bAttackNeutrals = bAutoAttackNeutrals;
	ACppRTSPlayerController();
}
ACppRTSPlayerController::ACppRTSPlayerController(FName OwnershipTag, bool bAutoAttackNeutrals) {
	OwnedTag = OwnershipTag;
	bAttackNeutrals = bAutoAttackNeutrals;
	ACppRTSPlayerController();
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
		EnhancedInputComponent->BindAction(LMBAction, ETriggerEvent::Started, this, &ACppRTSPlayerController::OnLMBDown);
		EnhancedInputComponent->BindAction(LMBAction, ETriggerEvent::Completed, this, &ACppRTSPlayerController::OnLMBUp);
		EnhancedInputComponent->BindAction(LMBAction, ETriggerEvent::Canceled, this, &ACppRTSPlayerController::OnLMBUp);
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
		Command(Hit, ECommand::Move, bShift);

		ACppRTSCharacter* HitUnit = Cast<ACppRTSCharacter>(Hit.GetActor());
		if (HitUnit != nullptr && HitUnit->OwningPlayer != nullptr) {
			EDiplomacy HitDiplomacy = URTSUtils::Diplomacy(GetWorld(), this, HitUnit->OwningPlayer);
			bool bEnemyClicked = (HitDiplomacy == EDiplomacy::Enemy || (bAttackNeutrals && HitDiplomacy == EDiplomacy::Neutral));
			if (bEnemyClicked) {
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, TEXT("Attack"));
				Command(Hit, ECommand::Attack, true);
			}
		}
	}
}

void ACppRTSPlayerController::OnShiftUp() {
	bShift = false;
}

void ACppRTSPlayerController::OnShiftDown() {
	bShift = true;
}

void ACppRTSPlayerController::OnLMBDown() {
	HUD_ref->BeginSelect();
}

void ACppRTSPlayerController::OnLMBUp() {
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

	// Possess Units
	TArray<AActor*> OwnedActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ACppRTSCharacter::StaticClass(), OwnedTag, OwnedActors);
	for (int i=0;i<OwnedActors.Num();i++) {
		ACppRTSCharacter* Unit = Cast<ACppRTSCharacter>(OwnedActors[i]);
		if (Unit != nullptr) {
			OwnedUnits.AddUnique(Unit);
			Unit->OwningPlayer = this;
		}
	}
}

void ACppRTSPlayerController::Tick(float DeltaSeconds)
{
	// Call the base class
    Super::Tick(DeltaSeconds);

	// Update all of our units' movement (Here rather than on each unit to improve performance (?))
	for (int i=0;i<OwnedUnits.Num();i++) {
		ACppRTSCharacter* Unit = OwnedUnits[i];
		if (!Unit->Tasks.IsEmpty()) {
			switch (Unit->Tasks[0]) {
				case ECommand::Move:
					if (!Unit->LocationTargets.IsEmpty()) {
						Unit->AIC->MoveToLocation(Unit->LocationTargets[0], 80., false);
						if (Unit->GetActorLocation().Equals(Unit->LocationTargets[0], 100.)) {
							Unit->CompleteTask();
						}
					}
					break;
				case ECommand::Attack:
					break;
			}
		}
	}
}

/* Methods */

// Selection
void ACppRTSPlayerController::Select(ACppRTSCharacter *Unit) {
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, *FString::Printf(TEXT("%s"), *(Unit->GetName())));
	Selects.AddUnique(Unit);
	Unit->ReceiveSelect(this, true);
}
void ACppRTSPlayerController::Deselect(ACppRTSCharacter *Unit) {
	Selects.Remove(Unit);
	Unit->ReceiveSelect(this, false);
}
void ACppRTSPlayerController::ClearSelection() {
	for (int i=0;i<Selects.Num();i++) {
		Selects[i]->ReceiveSelect(this, false);
	}
	Selects.Empty();
}

// AI
void ACppRTSPlayerController::AssignMoveTargets(TArray<ACppRTSCharacter*> Units, FVector ClickLocation, bool bQueue) {
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
			FarUnits[i]->AddCommand(ECommand::Move, ClickLocation + (dir*stdev*0.25), bQueue, true);
		}

		UnitActors.Empty();
		for (int i=0;i<NearUnits.Num();i++) {
			UnitActors.Add(Cast<AActor>(NearUnits[i]));
		}
		AvgLoc = UGameplayStatics::GetActorArrayAverageLocation(UnitActors);
		for (int i=0;i<NearUnits.Num();i++) {
			NearUnits[i]->AddCommand(ECommand::Move, (NearUnits[i]->GetActorLocation() - AvgLoc) + ClickLocation, bQueue, true);
		}
	}

	else {
		for (int i=0;i<Units.Num();i++) {
			FVector unitLoc = Units[i]->GetActorLocation();
			FVector dir = FVector(unitLoc.X, unitLoc.Y, ClickLocation.Z) - ClickLocation;
			dir.Normalize(0.0001);
			Units[i]->AddCommand(ECommand::Move, ClickLocation + (dir*50.), bQueue, true);
		}
	}
}

// Commands
void ACppRTSPlayerController::Command(FHitResult Target, ECommand cmd, bool bQueue) {
	bool bDidNotHitObject = Cast<ACppRTSCharacter>(Target.GetActor()) == nullptr;

	FVector TrueTargetLoc;
	switch (cmd) {
		case ECommand::Move:
			TrueTargetLoc = bDidNotHitObject ? Target.Location : Target.GetActor()->GetActorLocation();
			if (!Selects.IsEmpty()) {
				for (int i=0;i<Selects.Num();i++) {
					Selects[i]->AddCommand(ECommand::Move, TrueTargetLoc, bQueue, false);
				}
				AssignMoveTargets(Selects, TrueTargetLoc, bQueue);

				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MoveTargetParticle, TrueTargetLoc);
			}
			break;
		case ECommand::Attack:
			if (bDidNotHitObject) {
				// Queue an attack-move
				if (!Selects.IsEmpty()) {
					for (int i=0;i<Selects.Num();i++) {
						Selects[i]->AddCommand(ECommand::Attack, Target.Location, bQueue, false);
					}
				}
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), AttackTargetParticle, Target.Location);
			}
			// Queue a targeted attack on the hit unit
			else {
				if (!Selects.IsEmpty()) {
					for (int i=0;i<Selects.Num();i++) {
						Selects[i]->AddCommand(ECommand::Attack, Cast<ACppRTSCharacter>(Target.GetActor()), bQueue, false);
					}
					
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), AttackTargetParticle, Target.GetActor()->GetActorLocation());
				}
			}
			break;
	}
}