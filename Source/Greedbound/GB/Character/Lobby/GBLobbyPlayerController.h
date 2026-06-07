// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GBLobbyPlayerController.generated.h"

UCLASS()
class GREEDBOUND_API AGBLobbyPlayerController : public APlayerController
{
    GENERATED_BODY()
    
protected:
    virtual void BeginPlay() override;
};
