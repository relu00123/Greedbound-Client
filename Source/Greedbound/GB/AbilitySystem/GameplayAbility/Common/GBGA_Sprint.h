// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GB/AbilitySystem/GameplayAbility/GBGA_Base.h"
#include "GBGA_Sprint.generated.h"

class UGBMovementStateComponent;

UCLASS()
class GREEDBOUND_API UGBGA_Sprint : public UGBGA_Base
{
    GENERATED_BODY()

public:
    UGBGA_Sprint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override final;
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override final;
    virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override final;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override final;
                 
private:         
    UPROPERTY()
    TObjectPtr<UGBMovementStateComponent> MovementStateComponent; 

    FTimerHandle RegenStaminaTimer;
};
