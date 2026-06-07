// Fill out your copyright notice in the Description page of Project Settings.

#include "GBGameMode.h"
#include "Game/GBGameState.h"
#include "Define/GBDefine.h"

void AGBGameMode::BeginPlay()
{
    Super::BeginPlay();

    AGBGameState* GBGameState = GetGameState<AGBGameState>(); 

    if (GBGameState)
    {
        EGBSessionMode Mode;

        if (IsRunningDedicatedServer())
        {
            Mode = EGBSessionMode::DedicatedServer;
        }

        else
        {
            Mode = EGBSessionMode::LocalTest;
        }

        GBGameState->SetSessionMode(Mode);

        GB_BACKEND_LOG(Warning, TEXT("[GBGameMode] SessionMode = %s"), *UEnum::GetValueAsString(Mode));
    }
}
