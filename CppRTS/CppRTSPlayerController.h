// Copyright Gedalya Gordon and Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "CppRTSCharacter.h"
#include "CppRTSHUD.h"
#include "CppRTSPlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;

UCLASS()
class ACppRTSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/* Functions */

	// Constructor
	ACppRTSPlayerController();

	// Selection
	void Select(ACppRTSCharacter *Unit);
	void Deselect(ACppRTSCharacter *Unit);
	void ClearSelection();

	/* Variables */

	// Time Threshold to know if it was a short press
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	// Shift value
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	bool bShift;

	// Selects
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<ACppRTSCharacter*> Selects;
	
	// FX Class that we will spawn when clicking
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	// MappingContext
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;
	
	// RMB Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* RMBAction;

	// LMB Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* LMBAction;

	// Shift Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* ShiftAction;
protected:
	virtual void SetupInputComponent() override;
	
	// Called at start.
	virtual void BeginPlay();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Input handlers for RMB action. */
	void OnRMBDown();
    void OnRMBReleased();

	/** Input handlers for LMB action. */
	void OnLMBStarted();
    void OnLMBReleased();

	/** Input handlers for Shift action. */
	void OnShiftUp();
	void OnShiftDown();

    /** Calculate the move-destinations of the commanded units. */
    void AssignMoveTargets(TArray<ACppRTSCharacter*> Units, FVector ClickLocation);
private:
	// HUD ref
	UPROPERTY(EditAnywhere)
	ACppRTSHUD* HUD_ref;

	FInputModeGameAndUI InputMode;
};


