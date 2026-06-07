// Fill out your copyright notice in the Description page of Project Settings.

#include "GBGA_CastBuff.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "Define/GBDefine.h"
#include "Interface/GBCombatInterface.h"

void UGBGA_CastBuff::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
    {
        return;
    }

    GB_NULL_CHECK(CastMontage);

    if (GetAvatarActorFromActorInfo()->Implements<UGBCombatInterface>())
    {
        UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this,
            "PlayMontageAndWaitEvent",
            CastMontage,
            1.f);

        PlayMontageTask->OnInterrupted.AddDynamic(this, &UGBGA_CastBuff::K2_EndAbility);
        PlayMontageTask->OnCancelled.AddDynamic(this, &UGBGA_CastBuff::K2_EndAbility);
        PlayMontageTask->OnCompleted.AddDynamic(this, &UGBGA_CastBuff::ApplyGameEffect);
        PlayMontageTask->ReadyForActivation();
    }
}

void UGBGA_CastBuff::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGBGA_CastBuff::ApplyGameEffect()
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (ASC)
    {
        const FGameplayEffectSpecHandle& SpecHandle = ASC->MakeOutgoingSpec(SkillGameEffect, 1.f, ASC->MakeEffectContext());
        GB_CONDITION_CHECK(SpecHandle.IsValid());

        ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

        K2_EndAbility();
    }
}
