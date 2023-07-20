// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Math/Vector2D.h"
#include "CppRTSHUD.generated.h"

/**
 * 
 */
UCLASS()
class ACppRTSHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	ACppRTSHUD();

	void BeginSelect();
	void EndSelect();

	UPROPERTY(EditAnywhere)
	class ACppRTSPlayerController* Controller_ref;
protected:
	virtual void DrawHUD() override;
private:
	UPROPERTY(EditAnywhere, Category=Input)
	TArray<class ACppRTSCharacter*> ProspectiveSelects;

	UPROPERTY(EditAnywhere)
	FVector2D ClickPoint;
};