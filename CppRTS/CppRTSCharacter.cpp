// Copyright Gedalya Gordon and Epic Games, Inc. All Rights Reserved.

#include "CppRTSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "AIController.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "RTSUtils.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/DecalComponent.h"
#include "Materials/Material.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

ACppRTSCharacter::ACppRTSCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Initialize the PathingCollisionComponent
	// PathingCollisionComponent = CreateDefaultSubobject<USphereComponent>("Pathing Collision Component");
	// PathingCollisionComponent->SetupAttachment(RootComponent);
	// PathingCollisionComponent->RegisterComponent();
	// PathingCollisionComponent->SetRelativeLocation(FVector::ZeroVector);

}

void ACppRTSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Configure Hit event
	//PathingCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ACppRTSCharacter::OnCloseToOther);

	SpawnDefaultController();
	AIC = Cast<AAIController>(GetController());
}

void ACppRTSCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
	
	/* 

	// Moved to controller to improve performance (?)
	if (!LocationTargets.IsEmpty()) {
		AIC->MoveToLocation(LocationTargets[0], 80., false);
		if (GetActorLocation().Equals(LocationTargets[0], 100.)) {
			LocationTargets.RemoveAt(0);
			CommandTargets.RemoveAt(0);
		}
	} */
}

// void ACppRTSCharacter::OnCloseToOther_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
// 	FVector vel = GetMovementComponent()->Velocity;
// 	if (UKismetMathLibrary::GetMaxElement(vel) > 2.5) {
// 		ACppRTSCharacter* OtherUnit = Cast<ACppRTSCharacter>(OtherActor);
// 		if (OtherUnit != nullptr) {
// 			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, TEXT("Blocked by unit"));
// 			// Calculate the avoidance direction (perpendicular to our movement direction)
// 			FVector AvoidanceDirection = FVector(-vel.Y, vel.X, 0.).GetSafeNormal2D();
// 			OtherUnit->AddImmediateDestination(OtherActor->GetActorLocation()+(AvoidanceDirection*80.), true);
// 			OtherUnit->AddImmediateDestination(OtherActor->GetActorLocation()+(AvoidanceDirection*80.), false);
// 		}
// 	}
// }

void ACppRTSCharacter::ReceiveSelect(ACppRTSPlayerController *Selector, bool bNewSelectionStatus) {
	UDecalComponent* SelectionCircle = GetComponentByClass<UDecalComponent>();
	if (SelectionCircle) {
		switch (URTSUtils::Diplomacy(GetWorld(), Selector, OwningPlayer)) {
			// Set the color of the SelectionCircle 
			case EDiplomacy::Self:
				break;
		}
		SelectionCircle->SetVisibility(bNewSelectionStatus);
	}
}

void ACppRTSCharacter::AddCommand(ECommand cmd, FVector Destination, bool bAddReplace, bool bActionTarget) {
	if (!bAddReplace) { Tasks.Empty(); }
	switch (cmd) {
		case ECommand::Move:
			Tasks.Add(ECommand::Move);
			if (bActionTarget) {
				if (!bAddReplace) { LocationTargets.Empty(); }
				LocationTargets.Add(Destination);
			} else {
				if (!bAddReplace) {	CommandTargets.Empty();	}
				CommandTargets.Add(Destination);
			}
			break;
		case ECommand::Attack:
			//attack-move
			break;
	}
}
void ACppRTSCharacter::AddCommand(ECommand cmd, ACppRTSCharacter *TargetUnit, bool bAddReplace, bool bActionTarget) {
	switch (cmd) {
		case ECommand::Move:
			if (bActionTarget) {
				FVector Destination = TargetUnit->GetActorLocation() - ((TargetUnit->GetActorLocation() - GetActorLocation()) * (TargetUnit->GetCapsuleComponent()->GetScaledCapsuleRadius() + GetCapsuleComponent()->GetScaledCapsuleRadius() + 10.));
				AddCommand(ECommand::Move, Destination, bAddReplace, true);
			} else {
				AddCommand(ECommand::Move, TargetUnit->GetActorLocation(), bAddReplace, false);
			}
			break;
		case ECommand::Attack:
			if (!bAddReplace) { Tasks.Empty(); UnitTargets.Empty(); }
			Tasks.Add(ECommand::Attack);
			UnitTargets.Add(TargetUnit);
			break;
	}
}

void ACppRTSCharacter::CompleteTask() {
	switch (Tasks[0]) {
		case ECommand::Move:
			LocationTargets.RemoveAt(0);
			CommandTargets.RemoveAt(0);
			break;
		case ECommand::Attack:
			UnitTargets.RemoveAt(0);
			break;
		case ECommand::Build:
			UnitTargets.RemoveAt(0);
			Tasks.RemoveAt(0);

			FVector BuildingExitDest = FVector();
			Tasks.Insert(ECommand::Move, 0);
			LocationTargets.Insert(BuildingExitDest, 0);
			return;
	}
	Tasks.RemoveAt(0);
}