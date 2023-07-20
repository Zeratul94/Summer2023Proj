// Copyright Gedalya Gordon and Epic Games, Inc. All Rights Reserved.


#include "CppRTSHUD.h"

void ACppRTSHUD::BeginSelect() {
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, TEXT("Down"));
}

void ACppRTSHUD::EndSelect() {
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Up"));
}