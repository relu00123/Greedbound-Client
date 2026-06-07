// Fill out your copyright notice in the Description page of Project Settings.

#include "GBGA_PlayerUnequippedAttack.h"
#include "AbilitySystemComponent.h"

void UGBGA_PlayerUnequippedAttack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    Super::InputPressed(Handle, ActorInfo, ActivationInfo);

    if (IsPredictingClient())
    {
        UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
        if (ASC)
        {
            ASC->ServerSetInputPressed(Handle);
            SetContinueCombo(true);
        }
    }
    else if(HasAuthority(&ActivationInfo))
    {
        SetContinueCombo(true);
    }
}
