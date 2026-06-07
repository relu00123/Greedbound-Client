// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem\GameplayAbility\Common\GBGA_ComboAttack.h"
#include "GameplayTagContainer.h"
#include "GBGA_UnequippedAttack.generated.h"

UCLASS()
class GREEDBOUND_API UGBGA_UnequippedAttack : public UGBGA_ComboAttack
{
    GENERATED_BODY()

public:
    UGBGA_UnequippedAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void OnEventReceived(const FGameplayTag EventTag, FGameplayEventData EventData) override;

public:
    UPROPERTY(EditDefaultsOnly)
    float        AttackRange = 150.f;
};
