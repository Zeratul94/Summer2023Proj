// Copyright Gedalya Gordon and Epic Games, Inc. All Rights Reserved.

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

	// Called at start.
	virtual void BeginPlay() override;
	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

    void AddDestination(FVector Destination, bool bAddReplace, bool bMoveTarget);

	/** Destinations to go to */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FVector> MoveDestinations;
	/** Destinations we have been sent to **/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FVector> CommandDestinations;
private:
	class AAIController* AIC;
};

