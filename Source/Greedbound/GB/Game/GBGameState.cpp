// Fill out your copyright notice in the Description page of Project Settings.

#include "GBGameState.h"
#include "Net/UnrealNetwork.h"

AGBGameState::AGBGameState()
{
    SessionMode = EGBSessionMode::Unknown;
}

void AGBGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AGBGameState, SessionMode);
}
