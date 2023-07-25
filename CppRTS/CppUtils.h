// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CppUtils.generated.h"

/**
 * 
 */

// Command types that can be issued to a unit
UENUM(BlueprintType)
enum class ECommand : uint8
{
	Move,
	Attack,
	Build,
	HoldPosition
};

UCLASS()
class CPPRTS_API UCppUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
};
