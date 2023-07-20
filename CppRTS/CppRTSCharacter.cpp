// Copyright Gedalya Gordon and Epic Games, Inc. All Rights Reserved.

#include "CppRTSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "AIController.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
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

}

void ACppRTSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnDefaultController();
	AIC = Cast<AAIController>(GetController());
}

void ACppRTSCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (MoveDestinations.Num() > 0) {
		AIC->MoveToLocation(MoveDestinations[0], 80., false);
		if (GetActorLocation().Equals(MoveDestinations[0], 100.)) {
			MoveDestinations.RemoveAt(0);
			CommandDestinations.RemoveAt(0);
		}
	}
}

void ACppRTSCharacter::AddDestination(FVector Destination, bool bAddReplace, bool bMoveTarget) {
	if (bMoveTarget) {
		if (!bAddReplace) {	MoveDestinations.Empty();	}
		MoveDestinations.Add(Destination);
	} else {
		if (!bAddReplace) {	CommandDestinations.Empty();	}
		CommandDestinations.Add(Destination);
	}
}
