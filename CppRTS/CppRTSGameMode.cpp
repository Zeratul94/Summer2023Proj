// Copyright Epic Games, Inc. All Rights Reserved.

#include "CppRTSGameMode.h"
#include "CppRTSPlayerController.h"
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
}