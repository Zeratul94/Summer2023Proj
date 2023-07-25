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
	ACppRTSPlayerController(FName OwnershipTag);

	// Selection
	void Select(TArray<ACppRTSCharacter*> Units);
	void Select(ACppRTSCharacter* Unit);

	void Deselect(ACppRTSCharacter *Unit);
	
	void ClearSelection();

	/* Variables */

	// Time Threshold to know if it was a short press
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	// Shift value
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	bool bShift;

	// Tag to identify owned units
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName OwnedTag = FName(TEXT("Player1"));

	// Selects
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<ACppRTSCharacter*> Selects;
	
	// FX Class that we will spawn when a command is issued
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* MoveTargetParticle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* AttackTargetParticle;

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
	void OnLMBDown();
    void OnLMBUp();

	/** Input handlers for Shift action. */
	void OnShiftUp();
	void OnShiftDown();

	/** Give a command to the selected units */
	void Command(FHitResult Target, ECommand cmd, bool bQueue = false);

    /** Calculate the move-destinations of the commanded units. DOES NOT YET ACCOUNT FOR TARGET BEING ON A BUILDING */
    void AssignMoveTargets(TArray<ACppRTSCharacter*> Units, FVector ClickLocation, bool bQueue = false);
private:
	// HUD ref
	UPROPERTY(EditAnywhere)
	ACppRTSHUD* HUD_ref;

	// The units this Controller owns
	UPROPERTY(EditAnywhere)
	TArray<ACppRTSCharacter*> OwnedUnits;

	// Input Mode
	FInputModeGameAndUI InputMode;
};


