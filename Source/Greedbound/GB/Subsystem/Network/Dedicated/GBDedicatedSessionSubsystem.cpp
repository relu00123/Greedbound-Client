// Fill out your copyright notice in the Description page of Project Settings.

#include "GBDedicatedSessionSubsystem.h"
#include "Define/GBDefine.h"

void UGBDedicatedSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    bIsDungeonExists = false;
    DungeonToken = TEXT("");
    DungeonId = TEXT("");
}

void UGBDedicatedSessionSubsystem::StoreDungeonSession(const FString& InDungeonToken, const FString& InDungeonId)
{
    DungeonToken = InDungeonToken;
    DungeonId = InDungeonId;
    bIsDungeonExists = true;
    GB_BACKEND_LOG(Warning, TEXT("[DedicatedSession] Stored Token: %s, DungeonId: %s"), *DungeonToken, *DungeonId);
}

void UGBDedicatedSessionSubsystem::ClearDungeonSession()
{
    DungeonToken.Empty();
    DungeonId.Empty();
    bIsDungeonExists = false;
    GB_BACKEND_LOG(Warning, TEXT("[DedicatedSession] Cleared session data"));
}

 
