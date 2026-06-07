// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/GameplayAbility/GBGA_Base.h"
#include "GBGA_ComboAttack.generated.h"

class UAnimMontage;
class UGBAT_PlayMontageAndWaitForEvent;
class UGBCombatComponent;

UCLASS()
class GREEDBOUND_API UGBGA_ComboAttack : public UGBGA_Base
{
    GENERATED_BODY()

public:
    UGBGA_ComboAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void                                    ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void                                    EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
    FName                                           GetNextSectionNameByIndex();
    void                                            SetContinueCombo(const bool bTrigger);
    void                                            RotateCharacterToControllerRotation();

    UFUNCTION()
    virtual void                                    OnCompleted(const FGameplayTag EventTag, FGameplayEventData EventData);

    UFUNCTION()
    virtual void                                    OnEventReceived(const FGameplayTag EventTag, FGameplayEventData EventData);

    UFUNCTION()
    virtual void                                    OnTargetDataReady(const FGameplayAbilityTargetDataHandle& DataHandle);

private:
    float                                           GetHitDirectionToFloat(const FHitResult& Hit);

protected:
    UPROPERTY(Transient)
    TObjectPtr<UGBCombatComponent>                  CombatComponent;
    
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UGameplayEffect>                    DamageGameplayEffect;

    UPROPERTY()
    TObjectPtr<UGBAT_PlayMontageAndWaitForEvent>    PlayMontageAndWaitEventTask;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UAnimMontage>                        ComboAttackMontage;

    FGameplayTagContainer                           EventReceiveTags;

    int32                                           CurrentComboIndex           = 0;

    bool                                            bContinueCombo              = false;
    bool                                            bShouldRotateNextComboStart = true;
};
