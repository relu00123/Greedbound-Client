// Fill out your copyright notice in the Description page of Project Settings.

#include "GBSpeedAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "Define/GBTags.h"

void UGBSpeedAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    if (Attribute == GetCurrnetStaminaAttribute())
    {
        NewValue = NewValue <= 0.f ? 0.f : NewValue;
    }
}

void UGBSpeedAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
    if (OldValue <= 0.f)
    {
        return;
    }

    if (Attribute == GetCurrnetStaminaAttribute())
    {
        if (NewValue <= 0.f)
        {
            UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
            if (ASC && ASC->GetOwnerRole() == ENetRole::ROLE_Authority)
            {
                FGameplayTagContainer CancelTags = FGameplayTagContainer(GBTag::Event_Common_StaminaEmpty);
                ASC->CancelAbilities(&CancelTags);
            }
        }
    }
}

void UGBSpeedAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UGBSpeedAttributeSet, WalkSpeed);
    DOREPLIFETIME(UGBSpeedAttributeSet, SprintSpeed);
    DOREPLIFETIME(UGBSpeedAttributeSet, StaminaCost);
    DOREPLIFETIME(UGBSpeedAttributeSet, SprintStaminaThreshould);
    DOREPLIFETIME(UGBSpeedAttributeSet, CrouchSpeedCoefficient);

    DOREPLIFETIME_CONDITION_NOTIFY(UGBSpeedAttributeSet, CurrentSpeed, ELifetimeCondition::COND_None, ELifetimeRepNotifyCondition::REPNOTIFY_OnChanged);
    DOREPLIFETIME_CONDITION_NOTIFY(UGBSpeedAttributeSet, CurrnetStamina, ELifetimeCondition::COND_None, ELifetimeRepNotifyCondition::REPNOTIFY_OnChanged);
    DOREPLIFETIME_CONDITION_NOTIFY(UGBSpeedAttributeSet, MaxStamina, ELifetimeCondition::COND_None, ELifetimeRepNotifyCondition::REPNOTIFY_OnChanged);
}

void UGBSpeedAttributeSet::OnRep_CurrnetSpeed(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGBSpeedAttributeSet, CurrentSpeed, OldValue);
}

void UGBSpeedAttributeSet::OnRep_CurrnetStamina(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGBSpeedAttributeSet, CurrnetStamina, OldValue);
}

void UGBSpeedAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGBSpeedAttributeSet, MaxStamina, OldValue);
}
