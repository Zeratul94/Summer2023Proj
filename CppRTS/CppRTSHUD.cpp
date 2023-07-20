// Copyright Gedalya Gordon and Epic Games, Inc. All Rights Reserved.


#include "CppRTSHUD.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CppRTSPlayerController.h"
#include "Math/Vector2D.h"
#include "Blueprint/WidgetLayoutLibrary.h"

ACppRTSHUD::ACppRTSHUD()
{
    // Initialize the ClickPoint with the information that we are not selecting
    ClickPoint = FVector2D(-4096., -4096.);
}

void ACppRTSHUD::BeginSelect() {
    // Store the clicked location in ClickPoint
    ClickPoint = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld()) * UWidgetLayoutLibrary::GetViewportScale(GetWorld());
}

void ACppRTSHUD::EndSelect() {
    // Store in the ClickPoint that we are no longer selecting
    ClickPoint = FVector2D(-4096., -4096.);

    if (ProspectiveSelects.Num() > 0) {
        // If Shift is held, modify the current selection.
        if (Controller_ref->bShift) {
            // If multiple units are boxed, select all of them.
            if (ProspectiveSelects.Num() > 1) {
                GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("%i"), ProspectiveSelects.Num()));
                for (int i=0;i<ProspectiveSelects.Num();i++) {
                    Controller_ref->Select(ProspectiveSelects[i]);
                }
            }
            // If only one unit is shift-clicked...
            else {
                // If it is selected, deselect it.
                if (Controller_ref->Selects.Contains(ProspectiveSelects[0])) {
                    Controller_ref->Deselect(ProspectiveSelects[0]);
                }
                // Otherwise, select it.
                else {
                    Controller_ref->Select(ProspectiveSelects[0]);
                }
            }
        }
        // If Shift is not held, replace the current selection with all boxed units.
        else {
            Controller_ref->ClearSelection();
            for (int i=0;i<ProspectiveSelects.Num();i++) {
                Controller_ref->Select(ProspectiveSelects[i]);
            }
        }
    }

    // Clear our ProspectiveSelects to save memory (?)
    ProspectiveSelects.Empty();
}

void ACppRTSHUD::DrawHUD()
{
    Super::DrawHUD();

    if (ClickPoint.X >= -2048. && ClickPoint.Y >= -2048.) {
        // Store the current cursor location in DragPoint
        FVector2D DragPoint = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld()) * UWidgetLayoutLibrary::GetViewportScale(GetWorld());

        // Draw a bordered rectangle between ClickPoint and DragPoint
        DrawRect(FLinearColor(1., 1., 1., 0.125), ClickPoint.X, ClickPoint.Y, DragPoint.X - ClickPoint.X, DragPoint.Y - ClickPoint.Y);
        DrawLine(ClickPoint.X, ClickPoint.Y, DragPoint.X, ClickPoint.Y, FLinearColor::Green, 1.);
        DrawLine(ClickPoint.X, ClickPoint.Y, ClickPoint.X, DragPoint.Y, FLinearColor::Green, 1.);
        DrawLine(ClickPoint.X, DragPoint.Y, DragPoint.X, DragPoint.Y, FLinearColor::Green, 1.);
        DrawLine(DragPoint.X, ClickPoint.Y, DragPoint.X, DragPoint.Y, FLinearColor::Green, 1.);

        // Identify boxed units and store them in ProspectiveSelects
        ProspectiveSelects.Empty();
        GetActorsInSelectionRectangle<ACppRTSCharacter>(ClickPoint, DragPoint, ProspectiveSelects, false, true);
    }
}