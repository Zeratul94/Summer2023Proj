// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CppRTSHUD.generated.h"

/**
 * 
 */
UCLASS()
class ACppRTSHUD : public AHUD// , public IRTS_Select
{
	GENERATED_BODY()
	
public:
	void BeginSelect();
	void EndSelect();
};