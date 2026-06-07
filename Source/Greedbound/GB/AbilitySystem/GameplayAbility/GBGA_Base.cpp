// Fill out your copyright notice in the Description page of Project Settings.

#include "GBGA_Base.h"
#include "Components/GBAbilitySystemComponent.h"
#include "Define/GBDefine.h"
#include "Define/GBTags.h"
#include "Data/GBInputData.h"

UGBGA_Base::UGBGA_Base(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    TargetBlockedTags.AddTag(GBTag::Block_All);
}

const FGameplayTagContainer& UGBGA_Base::GetActivationOwnedTags()
{
    return ActivationOwnedTags;
}

void UGBGA_Base::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    UGBAbilitySystemComponent* ASC = Cast<UGBAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
    GB_NULL_CHECK(ASC);

    EGBTriggerType AbilityTriggerType = ASC->GetAbilityTriggerTypeByAssetTag(GetFirstAssetTag());
    if (AbilityTriggerType == EGBTriggerType::Toggle)
    {
        const FGameplayTagContainer& AssetTags = GetAssetTags();
        if (ASC->HasAllMatchingGameplayTags(AssetTags) == false)
        {
            ASC->AddLooseGameplayTags(AssetTags);
        }
    }
}

void UGBGA_Base::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    if (GetTriggerType() != EGBTriggerType::Toggle)
    {
        return;
    }

    UGBAbilitySystemComponent* ASC = Cast<UGBAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
    GB_NULL_CHECK(ASC);

    const FGameplayTagContainer& AssetTags = GetAssetTags();
    if (IsPredictingClient())
    {
        if (ASC->HasAllMatchingGameplayTags(AssetTags))
        {
            ASC->RemoveLooseGameplayTags(AssetTags);
            ASC->ServerSetInputReleased(Handle);
        }
    }
    else if (HasAuthority(&ActivationInfo))
    {
        if (ASC->HasAllMatchingGameplayTags(AssetTags))
        {
            ASC->RemoveLooseGameplayTags(AssetTags);
        }

        OnInputReleased();
    }
}

EGBTriggerType UGBGA_Base::GetTriggerType() const
{
    UGBAbilitySystemComponent* ASC = Cast<UGBAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
    GB_NULL_CHECK_WITH_RETURN(ASC, EGBTriggerType::None);

    return ASC->GetAbilityTriggerTypeByAssetTag(GetFirstAssetTag());
}

void UGBGA_Base::RemoveActiveEffectsByGrantedTag(const FGameplayTagContainer& Tags)
{
    GB_CONDITION_CHECK(Tags.IsValid());

    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    GB_NULL_CHECK(ASC);

    ASC->RemoveActiveEffectsWithGrantedTags(Tags);
    
}

void UGBGA_Base::RemoveActiveEffectsByGrantedTag(const FGameplayTag& Tag)
{
    RemoveActiveEffectsByGrantedTag(FGameplayTagContainer(Tag));
}

FGameplayTag UGBGA_Base::GetFirstAssetTag() const
{
    const FGameplayTagContainer& AssetTags = GetAssetTags();
    return AssetTags.IsEmpty() ? FGameplayTag::EmptyTag : AssetTags.GetByIndex(0);
}
