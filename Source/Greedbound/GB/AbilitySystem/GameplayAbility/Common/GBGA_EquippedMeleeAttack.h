// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/Common/GBGA_ComboAttack.h"
#include "GameplayTagContainer.h"
#include "GBGA_EquippedMeleeAttack.generated.h"

class UGBCombatComponent;
class UGBAT_WaitMeleeAttackTargetData;
class AGameplayAbilityTargetActor;

UCLASS()
class GREEDBOUND_API UGBGA_EquippedMeleeAttack : public UGBGA_ComboAttack
{
    GENERATED_BODY()

public:
    UGBGA_EquippedMeleeAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void                                OnEventReceived(const FGameplayTag EventTag, FGameplayEventData EventData) override;

protected:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AGameplayAbilityTargetActor>    MeleeAttackTargetActorClass;

    UPROPERTY()
    TObjectPtr<UGBAT_WaitMeleeAttackTargetData> WaitMeleeAttackTargetDataTask;
};
