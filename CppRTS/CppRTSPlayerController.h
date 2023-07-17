// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "CppRTSCharacter.h"
#include "CppRTSPlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;

UCLASS()
class ACppRTSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACppRTSPlayerController();

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;
	
	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;
	
	/** Click Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* SetDestinationClickAction;

	/** Shift Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* ShiftAction;

	bool bShift;
protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	virtual void SetupInputComponent() override;
	
	// Called at start.
	virtual void BeginPlay();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Input handlers for SetDestination action. */
	void OnSetDestinationStarted();
    void OnSetDestinationTriggered();
    void OnSetDestinationReleased();

	/** Input handlers for Shift action. */
	void OnShiftUp();
	void OnShiftDown();
	
	/** Calculate the move-destinations of the commanded units. */
	void AssignMoveTargets(TArray<ACppRTSCharacter*> Units, FVector ClickLocation);
private:
	FVector CachedDestination;
	UPROPERTY(EditAnywhere)
	TArray<ACppRTSCharacter*> Selects;

	bool bIsTouch; // Is it a touch device
	float FollowTime; // For how long it has been pressed
};


