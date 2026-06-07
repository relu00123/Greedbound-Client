// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/Main/GBLobbyMatchStartButton.h"
#include "UI/Lobby/Main/GBLobbyMatchStartButtonData.h"
#include "Define/Match/GBMatchQueueTypes.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UGBLobbyMatchStartButton::ApplyState(EGBLobbyMatchQueueState NewState)
{
    if (!Data || !StartButton || !Label) return;

    const FGBLobbyMatchStartButtonVisual V = Data->GetVisual(NewState);
    StartButton->SetIsEnabled(V.bEnabled);
    Label->SetText(V.ButtonText);
}

void UGBLobbyMatchStartButton::NativeConstruct()
{
    Super::NativeConstruct();

    if (StartButton)
    {
        StartButton->OnClicked.AddDynamic(this, &UGBLobbyMatchStartButton::HandleInternalClicked);
    }
}


void UGBLobbyMatchStartButton::HandleInternalClicked()
{
    OnMatchStartClicked.Broadcast();
}
