// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/Common/GBGA_UnequippedAttack.h"
#include "GBGA_PlayerUnequippedAttack.generated.h"

UCLASS()
class GREEDBOUND_API UGBGA_PlayerUnequippedAttack : public UGBGA_UnequippedAttack
{
    GENERATED_BODY()

public:    
    virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override final;
};
