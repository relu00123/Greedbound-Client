#include "GBAbilitySystemHelper.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Components/GBAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayAbilitySpec.h"
#include "Define/GBDefine.h"

void FGBAbilitySystemHelper::SendGameplayEventToTarget(const FGameplayTag EventTag, AActor* Instigator, AActor* Target, const float Magnitude /*= 0.f*/)
{
    GB_NULL_CHECK(Instigator);
    GB_NULL_CHECK(Target);

    UAbilitySystemComponent* TargetASC = GetAbilitySystemComponentFromActor(Target);
    GB_NULL_CHECK(TargetASC);

    FGameplayEventData EventData = GetDefaultGameplayEventData(EventTag, Target, Instigator);
    EventData.EventMagnitude = Magnitude;

    TargetASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}

void FGBAbilitySystemHelper::SendGameplayEventToTarget(const FGameplayTag EventTag, AActor* Instigator, AActor* Target, UObject* Object)
{
    GB_NULL_CHECK(Instigator);
    GB_NULL_CHECK(Target);

    UAbilitySystemComponent* TargetASC = GetAbilitySystemComponentFromActor(Target);
    GB_NULL_CHECK(TargetASC);

    FGameplayEventData EventData = GetDefaultGameplayEventData(EventTag, Target, Instigator);
    EventData.OptionalObject = Object;

    TargetASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}

void FGBAbilitySystemHelper::SendGameplayEventToSelf(const FGameplayTag EventTag, AActor* SourceActor, const float Magnitude /*= 0.f*/)
{

    GB_NULL_CHECK(SourceActor);

    UAbilitySystemComponent* Source = GetAbilitySystemComponentFromActor(SourceActor);
    GB_NULL_CHECK(Source);

    FGameplayEventData EventData = GetDefaultGameplayEventData(EventTag, SourceActor);
    EventData.EventMagnitude = Magnitude;

    Source->HandleGameplayEvent(EventData.EventTag, &EventData);
}

void FGBAbilitySystemHelper::SendGameplayEventToTarget(const FGameplayTag EventTag, AActor* SourceActor, UObject* Object)
{
    GB_NULL_CHECK(SourceActor);

    UAbilitySystemComponent* Source = GetAbilitySystemComponentFromActor(SourceActor);
    GB_NULL_CHECK(Source);

    FGameplayEventData EventData = GetDefaultGameplayEventData(EventTag, SourceActor);
    EventData.OptionalObject = Object;

    Source->HandleGameplayEvent(EventData.EventTag, &EventData);
}

bool FGBAbilitySystemHelper::HasMatchingGameplayTag(const FGameplayTag TagToCheck, AActor* TargetActor)
{
    GB_VALID_CHECK_WITH_RETURN(TargetActor, false);

    UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
    GB_NULL_CHECK_WITH_RETURN(TargetASC, false);

    return TargetASC->HasMatchingGameplayTag(TagToCheck);
}

const FGameplayTagContainer& FGBAbilitySystemHelper::GetOwnedGameplayTag(AActor* TargetActor)
{   
    GB_VALID_CHECK_WITH_RETURN(TargetActor, FGameplayTagContainer::EmptyContainer);

    UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
    GB_VALID_CHECK_WITH_RETURN(TargetActor, FGameplayTagContainer::EmptyContainer);

    return TargetASC->GetOwnedGameplayTags();
}


UGBAbilitySystemComponent* FGBAbilitySystemHelper::GetAbilitySystemComponentFromActor(AActor* Actor)
{
    UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
    if (ASC)
    {
        UGBAbilitySystemComponent* GBASC = Cast<UGBAbilitySystemComponent>(ASC);
        if (GBASC)
        {
            return GBASC;
        }

        GB_LOG(TEXT("Actor ASC is not GBASC"));
    }

    return nullptr;
}

void FGBAbilitySystemHelper::GetAbilitySpecsByTag(UAbilitySystemComponent* ASC, const FGameplayTag AssetTags, TArray<FGameplayAbilitySpec*>& Outlist)
{
    GB_NULL_CHECK(ASC);
    Outlist.Reset();

    UGBAbilitySystemComponent* GBASC = Cast<UGBAbilitySystemComponent>(ASC);
    GB_NULL_CHECK(GBASC);

    GBASC->GetAbilitySpecsByAssetTag(AssetTags, Outlist);
}

FGameplayEventData FGBAbilitySystemHelper::GetDefaultGameplayEventData(const FGameplayTag EventTag, AActor* Target, AActor* Instigator /*= nullptr*/)
{
    FGameplayEventData EventData;
    EventData.EventTag = EventTag;
    EventData.Instigator = Instigator;
    EventData.Target = Target;

    return EventData;
}
