// Copyright Gedalya Gordon and Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RTSUtils.h"
#include "CppRTSGameMode.generated.h"

/** Forward declaration to improve compiling times */
class ACppRTSPlayerController;

UCLASS(minimalapi)
class ACppRTSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACppRTSGameMode();
	ACppRTSGameMode(TMap<FName, TMap<FName, EDiplomacy>> InDiplomacyMap);

	const uint8 NumPlayers = 1;
	TMap<ACppRTSPlayerController*, TMap<ACppRTSPlayerController*, EDiplomacy>> DiplomacyMatrix;
protected:
	// Called at start.
	void PostLogin(APlayerController* NewPlayer) override;
    void InitializeDiplomacy(TMap<FName, TMap<FName, EDiplomacy>> InMap);
	void UpdateDiplomacy(ACppRTSPlayerController* NewPlayer);

private:
	TMap<FName, TMap<FName, EDiplomacy>> DiplomacyMap;
    TArray<class ACppRTSPlayerController*> Players;
};