// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor_Trace.h"
#include "GBTA_Trace.generated.h"

UCLASS()
class GREEDBOUND_API AGBTA_Trace : public AGameplayAbilityTargetActor_Trace
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    virtual void StartTargeting(UGameplayAbility* Ability) override final;
};
