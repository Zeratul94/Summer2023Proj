// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSUtils.h"
#include "CppRTSGameMode.h"
#include "Kismet/GameplayStatics.h"

EDiplomacy URTSUtils::Diplomacy(UObject *WorldContextObject, ACppRTSPlayerController *Target, ACppRTSPlayerController* Other) {
    ACppRTSGameMode* GameMode = Cast<ACppRTSGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
    if (GameMode != nullptr) {
        return GameMode->DiplomacyMatrix[Target][Other];
    }
    return EDiplomacy::Neutral;// editor diplomacy check?
}