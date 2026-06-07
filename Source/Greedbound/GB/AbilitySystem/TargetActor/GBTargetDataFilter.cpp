// Fill out your copyright notice in the Description page of Project Settings.

#include "GBTargetDataFilter.h"
#include "AbilitySystem/GBAbilitySystemHelper.h"
#include "Define/GBDefine.h"

bool FGBGameplayTargetDataFilter::FilterPassesForActor(const AActor* ActorToBeFiltered) const
{
    AActor* TargetActor = const_cast<AActor*>(ActorToBeFiltered);
    GB_NULL_CHECK_WITH_RETURN(TargetActor, false);

    const FGameplayTagContainer& TargetOwnedTags = FGBAbilitySystemHelper::GetOwnedGameplayTag(TargetActor);
    if (TargetOwnedTags.IsEmpty())
    {
        if (RequiredTags.IsEmpty() == false)
        {
            return false;
        }
    }
    else
    {
        if (RequiredTags.IsEmpty() == false && TargetOwnedTags.HasAll(RequiredTags))
        {
            return Super::FilterPassesForActor(ActorToBeFiltered);
        }

        if (FilterTags.IsEmpty() == false && TargetOwnedTags.HasAny(FilterTags))
        {
            return false;
        }
    }

    return Super::FilterPassesForActor(ActorToBeFiltered);
}
