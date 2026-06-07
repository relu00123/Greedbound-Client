// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GB/AbilitySystem/GameplayAbility/Common/GBGA_EquippedMeleeAttack.h"
#include "GBGA_PlayerEquippedAttack.generated.h"

UCLASS()
class GREEDBOUND_API UGBGA_PlayerEquippedAttack : public UGBGA_EquippedMeleeAttack
{
    GENERATED_BODY()

public:
    virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override final;
};
