// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GBDedicatedSessionSubsystem.generated.h"

UCLASS()
class GREEDBOUND_API UGBDedicatedSessionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:
    void                            StoreDungeonSession(const FString& InDungeonToken, const FString& InDungeonId);
    FORCEINLINE bool                IsDungeonExists() { return bIsDungeonExists; }
    FORCEINLINE const FString&      GetDungeonToken() { return DungeonToken; }
    FORCEINLINE const FString&      GetDungeonId() { return DungeonId;  }

protected:
    virtual void                    Initialize(FSubsystemCollectionBase& Collection) override;

private:
    void                            ClearDungeonSession();

private:
    FString                         DungeonToken;
    FString                         DungeonId;
    bool                            bIsDungeonExists; 
};
