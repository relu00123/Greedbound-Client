// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/Main/GBLobbyMatchStartButtonData.h"
#include "Define/Match/GBMatchQueueTypes.h"


FGBLobbyMatchStartButtonVisual UGBLobbyMatchStartButtonData::GetVisual(EGBLobbyMatchQueueState State) const
{
    if (const FGBLobbyMatchStartButtonVisual* Found = Visuals.Find(State))
    {
        return *Found; // 값 반환(복사)
    }
    return DefaultVisual;
}

