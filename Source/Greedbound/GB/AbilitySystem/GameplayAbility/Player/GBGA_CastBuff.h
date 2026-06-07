// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GBGA_PlayerSkill.h"
#include "GBGA_CastBuff.generated.h"

class UAnimMontage;

UCLASS()
class GREEDBOUND_API UGBGA_CastBuff : public UGBGA_PlayerSkill
{
    GENERATED_BODY()

public:
    virtual void                    ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void                    EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
    UFUNCTION()
    void                            ApplyGameEffect();

private:
    UPROPERTY(EditDefaultsOnly, Category = "SklillAbilityInfo")
    TSubclassOf<UGameplayEffect>    SkillGameEffect;

    UPROPERTY(EditDefaultsOnly, Category = "SklillAbilityInfo")
    TObjectPtr<UAnimMontage>        CastMontage;
};
