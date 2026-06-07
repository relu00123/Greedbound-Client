// Fill out your copyright notice in the Description page of Project Settings.

#include "GBHealthAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayAbilities/Public/GameplayEffectExtension.h"
#include "Define/GBTags.h"

bool UGBHealthAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
    if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
    {
        if (Data.Target.HasMatchingGameplayTag(GBTag::State_Buff_Invincible) && Data.EvaluatedData.Magnitude < 0.f)
        {
            return false;
        }

        if (Data.EvaluatedData.Magnitude > 0.f && GetCurrentHealth() >= GetMaxHealth())
        {
            return false;
        }
    }

    return true;
}

void UGBHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    if (Data.Target.GetOwnerRole() == ENetRole::ROLE_Authority)
    {
        if (Data.EvaluatedData.Attribute == GetHitDirectionAttribute() && Data.EvaluatedData.Magnitude > 0.f)
        {
            FGameplayEventData EventData;
            EventData.EventTag = GBTag::Trigger_Common_HitReact;
            EventData.Instigator = Data.Target.GetAvatarActor();
            EventData.EventMagnitude = GetHitDirection();

            Data.Target.HandleGameplayEvent(GBTag::Trigger_Common_HitReact, &EventData);
        }
    }
}

void UGBHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    if (Attribute == GetCurrentHealthAttribute())
    {
        NewValue = NewValue < 0.f ? 0.f : NewValue;
    }
    else if (Attribute == GetMaxHealthAttribute())
    {
        NewValue = NewValue < 0.f ? 0.f : NewValue;
    }
}

void UGBHealthAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
    if (OldValue <= 0.f)
    {
        return;
    }

    if (Attribute == GetCurrentHealthAttribute())
    {
        UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
        if (ASC && ASC->GetOwnerRole() == ENetRole::ROLE_Authority)
        {
            if (NewValue <= 0.f)
            {
                FGameplayEventData EventData;
                EventData.EventTag = GBTag::Trigger_Common_Dead;
                EventData.Instigator = ASC->GetAvatarActor();
                EventData.EventMagnitude = GetHitDirection();

                ASC->HandleGameplayEvent(GBTag::Trigger_Common_Dead, &EventData);
            }
        }
    }
}

void UGBHealthAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UGBHealthAttributeSet, CurrentHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UGBHealthAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME(UGBHealthAttributeSet, HitDirection);
}

void UGBHealthAttributeSet::OnRep_CurrentHealth(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGBHealthAttributeSet, CurrentHealth, OldValue);
}

void UGBHealthAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGBHealthAttributeSet, MaxHealth, OldValue);
}


