// Copyright Gedalya Gordon and Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "RTSUtils.h"
#include "CppRTSCharacter.generated.h"

UCLASS(Blueprintable)
class ACppRTSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACppRTSCharacter();

	/** VARIABLES */

	// Destinations to go to 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FVector> LocationTargets;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<ACppRTSCharacter*> UnitTargets;
	// Destinations we have been "sent to"
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FVector> CommandTargets;

	// Commands that have been issued to the unit, in order to execute
	TArray<ECommand> Tasks;

	// The player that owns this unit
	class ACppRTSPlayerController * OwningPlayer;

	// This unit's personal AI controller
	class AAIController* AIC;

	// The sphere collision that detects nearby units during pathing
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//USphereComponent* PathingCollisionComponent;

	/** FUNCTIONS */

	// Called at start.
	virtual void BeginPlay() override;
	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	void ReceiveSelect(ACppRTSPlayerController *Selector, bool bNewSelectionStatus);

    void AddCommand(ECommand Action, FVector Destination, bool bAddReplace, bool bActionTarget);
	void AddCommand(ECommand Action, ACppRTSCharacter *TargetUnit, bool bAddReplace, bool bActionTarget);
	void CompleteTask();
	//void AddImmediateDestination(FVector Destination, bool bActionTarget);
	//void JustStartedMovement();
	//void CheckBlockers();

	/* OnHit */
	//UFUNCTION(BlueprintNativeEvent)
	//void OnCloseToOther(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	//Default Implementation
	//virtual void OnCloseToOther_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};

