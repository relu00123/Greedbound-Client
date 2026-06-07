// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Dungeon/GBDungeonGameState.h"
#include "Game/Dungeon/GBDungeonGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Define/GBDefine.h"
#include "Character/Player/GBPlayerController.h"

void AGBDungeonGameState::AddDungeonMapLoadedPlayer()
{
    DungeonMapLoadedPlayerCount++;
    OnRep_DungeonMapLoadedPlayerCount();

    if (AGBDungeonGameMode* DungeonGameMode = GetWorld()->GetAuthGameMode<AGBDungeonGameMode>())
    {
        DungeonGameMode->HandleDungeonMapLoadedCountChanged();
    }
}

void AGBDungeonGameState::OnRep_MatchStarted()
{
    if (bMatchStarted)
    {
        // 대기 HUD 닫고 게임 HUD 켜기  (HandleMatchStarted_Client() 에서 진행) 
        if (GetNetMode() == NM_Client)
        {
            if (AGBPlayerController* PlayerController = GetWorld()->GetFirstPlayerController<AGBPlayerController>())
            {
                PlayerController->HandleMatchStarted_Client();
            }
        }
    }
}

void AGBDungeonGameState::OnRep_DungeonMapLoadedPlayerCount()
{
    ENetMode Mode = GetNetMode();
    if (Mode == NM_DedicatedServer || Mode == NM_ListenServer)
    {
        GB_BACKEND_LOG(Warning, TEXT("[GameState] LoadedPlayerCount updated: %d / %d"), DungeonMapLoadedPlayerCount, ExpectedPlayers);
    }
    else if (Mode == NM_Client)
    {
        GB_BACKEND_LOG(Warning, TEXT("[GameState] LoadedPlayerCount updated: %d / %d"), DungeonMapLoadedPlayerCount, ExpectedPlayers);
    }

    // 클라이언트에서 UI 업데이트 
}

void AGBDungeonGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AGBDungeonGameState, ExpectedPlayers);
    DOREPLIFETIME(AGBDungeonGameState, ConnectedPlayers);
    DOREPLIFETIME(AGBDungeonGameState, CountdownRemaining);
    DOREPLIFETIME(AGBDungeonGameState, DungeonMapLoadedPlayerCount); 
}
