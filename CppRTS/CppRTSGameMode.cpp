// Copyright Gedalya Gordon and Epic Games, Inc. All Rights Reserved.

#include "CppRTSGameMode.h"
#include "CppRTSPlayerController.h"
#include "RTSUtils.h"
#include "UObject/ConstructorHelpers.h"

ACppRTSGameMode::ACppRTSGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ACppRTSPlayerController::StaticClass();

	// set default pawn class to our CameraPawn
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/BlupContent/Blueprints/BP_TopDownCameraPawn"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/BlupContent/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	/* TEMP while diplomacy input not implemented */
	FName p1tag = FName(TEXT("Player1"));
	DiplomacyMap.Add(FName(TEXT("Player1")), TMap<FName, EDiplomacy>());
	DiplomacyMap[p1tag].Add(p1tag, EDiplomacy::Self);
}

ACppRTSGameMode::ACppRTSGameMode(TMap<FName, TMap<FName, EDiplomacy>> InDiplomacyMap) {
	DiplomacyMap = InDiplomacyMap;
	ACppRTSGameMode();
}

/* Initialize players */
void ACppRTSGameMode::PostLogin(APlayerController* NewPlayer)
{
    // Call the base class
    Super::PostLogin(NewPlayer);

    // Store the reference to the PlayerController in a list or TArray
    // For example, you can store it in a TArray of APlayerController pointers in the GameMode:
	ACppRTSPlayerController* Player = Cast<ACppRTSPlayerController>(NewPlayer);
    Players.Add(Player);

	// If all players have joined, initialize diplomacy and start the game.
	if (Players.Num() == NumPlayers) {
		InitializeDiplomacy(DiplomacyMap);
		// Start Game
	}
}

void ACppRTSGameMode::InitializeDiplomacy(TMap<FName, TMap<FName, EDiplomacy>> InMap) {
	for (int32 i = 0; i < Players.Num(); i++) {
		for (int32 j = 0; j < Players.Num(); j++) {
			// Create the  "row" in the TMap for Players[i] if it doesn't already exist
			if (!DiplomacyMatrix.Contains(Players[i])) {
				DiplomacyMatrix.Add(Players[i], TMap<ACppRTSPlayerController*, EDiplomacy>());
			}

			// Add item (i, j) with the proper diplomacy
			DiplomacyMatrix[Players[i]].Add(Players[j], (Players[i] == Players[j]) ? EDiplomacy::Self : InMap[Players[i]->OwnedTag][Players[j]->OwnedTag]);
		}
	}
}

void ACppRTSGameMode::UpdateDiplomacy(ACppRTSPlayerController* NewPlayer/* , diplomacy input single */) {
	DiplomacyMatrix.Add(NewPlayer, TMap<ACppRTSPlayerController*, EDiplomacy>());
	for (int32 i = 0; i < Players.Num(); i++) {
		if (Players[i] != NewPlayer) {
			// Create the  "row" in the TMap for Players[i] if it doesn't already exist
			if (!DiplomacyMatrix.Contains(Players[i])) {
				DiplomacyMatrix.Add(Players[i], TMap<ACppRTSPlayerController*, EDiplomacy>());
			}

			// Add item (i, NewIdx) with the proper diplomacy
			DiplomacyMatrix[Players[i]].Add(NewPlayer, /* read diplomacy from inputs */EDiplomacy::Neutral);
		}
		DiplomacyMatrix[NewPlayer].Add(Players[i], (Players[i] == NewPlayer) ? EDiplomacy::Self : /* read from inputs */EDiplomacy::Neutral);
	}
}