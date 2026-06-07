// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GBGameMode.generated.h"

UCLASS()
class GREEDBOUND_API AGBGameMode : public AGameMode
{
    GENERATED_BODY()
    
protected:
    virtual void BeginPlay() override; 
};
