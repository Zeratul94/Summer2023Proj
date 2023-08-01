// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RTSUtils.generated.h"

/** Forward declaration to improve compiling times */
class ACppRTSPlayerController;

// Command types that can be issued to a unit
UENUM(BlueprintType)
enum class ECommand : uint8
{
	Move,
	Attack,
	Build,
	HoldPosition
};

UENUM(BlueprintType)
enum class EDiplomacy : uint8
{
	Self,
	Ally,
	Neutral,
	Enemy
};

UCLASS()
class CPPRTS_API URTSUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static EDiplomacy Diplomacy(UObject *WorldContextObject, ACppRTSPlayerController* Target, ACppRTSPlayerController* Other);
};
