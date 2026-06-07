// Fill out your copyright notice in the Description page of Project Settings.

#include "GBLobbyHUD.h"
#include "GB/UI/Lobby/GBLobbyWidget.h"
#include "Blueprint/UserWidget.h"
#include "UI/Popup/RightClick/GBRightClickMenuWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"

void AGBLobbyHUD::BeginPlay()
{
    Super::BeginPlay();

    if (LobbyWidgetClass)
    {
        LobbyWidget = CreateWidget<UGBLobbyWidget>(GetWorld(), LobbyWidgetClass);
        if (LobbyWidget)
        {
            LobbyWidget->AddToViewport();
        }
    }
}
