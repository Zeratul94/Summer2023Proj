// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CppRTSCharacter.generated.h"

UCLASS(Blueprintable)
class ACppRTSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACppRTSCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

    void AddDestination(FVector Destination, bool bAddReplace, bool bMoveTarget);

    /** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Destinations to go to */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FVector> MoveDestinations;
	/** Destinations we have been sent to **/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FVector> CommandDestinations;
private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
};

