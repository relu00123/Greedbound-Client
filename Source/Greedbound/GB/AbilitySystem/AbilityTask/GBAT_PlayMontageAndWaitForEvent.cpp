// Fill out your copyright notice in the Description page of Project Settings.

#include "GBAT_PlayMontageAndWaitForEvent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "Define/GBDefine.h"

void UGBAT_PlayMontageAndWaitForEvent::Activate()
{
    Super::Activate();

    GB_NULL_CHECK(Ability);

    if (AbilitySystemComponent.IsValid())
    {
        const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
        UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
        if (AnimInstance)
        {
            EventHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTags,
                FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UGBAT_PlayMontageAndWaitForEvent::OnGameplayEvent));

            if (AbilitySystemComponent->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection) > 0.f)
            {
                if (ShouldBroadcastAbilityTaskDelegates() == false)
                {
                    return;
                }

                MontageEndedDelegate.BindUObject(this, &UGBAT_PlayMontageAndWaitForEvent::OnMontageEnded);
                AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

                ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
                if (Character && (Character->GetLocalRole() == ROLE_Authority ||
                    (Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
                {
                    Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("UGDAbilityTask_PlayMontageAndWaitForEvent call to PlayMontage failed!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UGDAbilityTask_PlayMontageAndWaitForEvent called on invalid AbilitySystemComponent"));
    }
}

void UGBAT_PlayMontageAndWaitForEvent::OnDestroy(bool AbilityEnded)
{
    if (Ability)
    {
        if (AbilityEnded && bStopWhenAbilityEnds)
        {
            StopPlayingMontage();
        }
    }

    if (AbilitySystemComponent.IsValid())
    {
        AbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
    }

    Super::OnDestroy(AbilityEnded);
}

UGBAT_PlayMontageAndWaitForEvent* UGBAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(UGameplayAbility* OwningAbility, const FName TaskInstanceName, UAnimMontage* MontageToPlay, FGameplayTagContainer EventTags, float Rate /*= 1.f*/, const FName StartSection /*= NAME_None*/, const bool bStopWhenAbilityEnds /*= true*/, const float AnimRootMotionTranslationScale /*= 1.f*/)
{
    UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

    UGBAT_PlayMontageAndWaitForEvent* MyObj = NewAbilityTask<UGBAT_PlayMontageAndWaitForEvent>(OwningAbility, TaskInstanceName);
    MyObj->MontageToPlay = MontageToPlay;
    MyObj->EventTags = EventTags;
    MyObj->Rate = Rate;
    MyObj->StartSection = StartSection;
    MyObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
    MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;

    return MyObj;
}

const bool UGBAT_PlayMontageAndWaitForEvent::StopPlayingMontage()
{
    GB_NULL_CHECK_WITH_RETURN(Ability, false);

    const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
    GB_NULL_CHECK_WITH_RETURN(ActorInfo, false);

    UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
    GB_NULL_CHECK_WITH_RETURN(AnimInstance, false);

    UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
    if (ASC)
    {
        if (ASC->GetAnimatingAbility() == Ability && ASC->GetCurrentMontage() == MontageToPlay)
        {
            // Unbind delegates so they don't get called as well
            FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
            if (MontageInstance)
            {
                MontageInstance->OnMontageEnded.Unbind();
            }

            ASC->CurrentMontageStop();
            return true;
        }
    }

    return false;
}

void UGBAT_PlayMontageAndWaitForEvent::JumpToSection(const FName SectionName)
{
    if (AbilitySystemComponent.IsValid())
    {
        const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
        UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
        if (AnimInstance)
        {
            AbilitySystemComponent->CurrentMontageJumpToSection(SectionName);
        }
    }
}

void UGBAT_PlayMontageAndWaitForEvent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (!bInterrupted)
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            OnCompleted.Broadcast(FGameplayTag::EmptyTag, FGameplayEventData());
        }
    }

    EndTask();
}

void UGBAT_PlayMontageAndWaitForEvent::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
    if (ShouldBroadcastAbilityTaskDelegates())
    {
        FGameplayEventData TempData = *Payload;
        TempData.EventTag = EventTag;

        OnEventReceived.Broadcast(EventTag, TempData);
    }
}
