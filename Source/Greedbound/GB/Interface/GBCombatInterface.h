// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GBCombatInterface.generated.h"

class UGBCombatComponent;

UINTERFACE(MinimalAPI)
class UGBCombatInterface : public UInterface
{
    GENERATED_BODY()
};

class GREEDBOUND_API IGBCombatInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent)
    UGBCombatComponent* GetCombatComponent() const;
};
