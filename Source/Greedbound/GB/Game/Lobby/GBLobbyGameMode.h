// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GBLobbyGameMode.generated.h"

UCLASS()
class GREEDBOUND_API AGBLobbyGameMode : public AGameMode
{
    GENERATED_BODY()
    
protected:
    virtual void        BeginPlay() override;
};
