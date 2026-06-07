// Fill out your copyright notice in the Description page of Project Settings.

#include "GBAbilitySystemComponent.h"
#include "Define/GBDefine.h"
#include "Data/GBInputData.h"
#include "Data/GBCharacterData.h"
#include "AbilitySystem/GameplayAbility/GBGA_Base.h"

UGBAbilitySystemComponent::UGBAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    static ConstructorHelpers::FClassFinder<UGameplayEffect> InitStatGameplayEffectRef(TEXT("/Game/GB/GAS/Effect/GE_Instant_InitStats.GE_Instant_InitStats_C"));
    if (InitStatGameplayEffectRef.Succeeded())
    {
        InitAttributeStatGameplayEffectClass = InitStatGameplayEffectRef.Class;
    }
}

void UGBAbilitySystemComponent::AbilityTagTriggered(const FGameplayTag AssetTag)
{
    GB_CONDITION_CHECK(AssetTag.IsValid());

    TArray<FGameplayAbilitySpec*> ActivatableAbilityList;
    GetAbilitySpecsByAssetTag(AssetTag, ActivatableAbilityList);
    for (FGameplayAbilitySpec* Spec : ActivatableAbilityList)
    {
        if (Spec->IsActive())
        {
            AbilitySpecInputPressed(*Spec);
        }
        else
        {
            TryActivateAbility(Spec->Handle);
        }
    }
}

void UGBAbilitySystemComponent::AbilityTagReleased(const FGameplayTag AssetTag)
{
    GB_CONDITION_CHECK(AssetTag.IsValid());

    TArray<FGameplayAbilitySpec*> ActivatableAbilityList;
    GetAbilitySpecsByAssetTag(AssetTag, ActivatableAbilityList);
    for (FGameplayAbilitySpec* Spec : ActivatableAbilityList)
    {
        if (Spec->IsActive())
        {
            AbilitySpecInputReleased(*Spec);
        }
    }
}

void UGBAbilitySystemComponent::AbilityTagToggled(const FGameplayTag AssetTag)
{
    GB_CONDITION_CHECK(AssetTag.IsValid());

    TArray<FGameplayAbilitySpec*> ActivatableAbilityList;
    GetAbilitySpecsByAssetTag(AssetTag, ActivatableAbilityList);
    for (FGameplayAbilitySpec* Spec : ActivatableAbilityList)
    {
        if (Spec == nullptr)
        {
            continue;
        }

        if (Spec->IsActive())
        {
            AbilitySpecInputReleased(*Spec);
        }
        else
        {
            TryActivateAbility(Spec->Handle);
        }
    }
}

void UGBAbilitySystemComponent::SetInputData(UGBInputData* NewInputData)
{
    GB_NULL_CHECK(NewInputData);
    InputData = NewInputData;
}

const FGameplayTagContainer& UGBAbilitySystemComponent::GetActivationOwnedTags(UGameplayAbility* Ability) const
{
    UGBGA_Base* GBAbility = Cast<UGBGA_Base>(Ability);
    GB_NULL_CHECK_WITH_RETURN(GBAbility, FGameplayTagContainer::EmptyContainer);

    return GBAbility->GetActivationOwnedTags();
}

EGBTriggerType UGBAbilitySystemComponent::GetAbilityTriggerTypeByAssetTag(FGameplayTag AssetTag) const
{
    GB_NULL_CHECK_WITH_RETURN(InputData, EGBTriggerType::None);
    GB_CONDITION_CHECK_WITH_RETURN(AssetTag.IsValid(), EGBTriggerType::None);
    return InputData->GetTriggerTypeByTag(AssetTag);
}

void UGBAbilitySystemComponent::GetAbilitySpecsByAssetTag(const FGameplayTag AssetTag, TArray<FGameplayAbilitySpec*>& Outlist)
{
    GB_CONDITION_CHECK(AssetTag.IsValid());

    TArray<FGameplayAbilitySpec>& ActivatebleAbilities = GetActivatableAbilities();
    for (FGameplayAbilitySpec& Spec : ActivatebleAbilities)
    {
        const FGameplayTagContainer& TagContainer = Spec.Ability->GetAssetTags();
        if (TagContainer.IsValid() && TagContainer.HasTagExact(AssetTag))
        {
            Outlist.Add(&Spec);
        }
    }
}

void UGBAbilitySystemComponent::GetGameplayEffectDurationByAssetTag(const FGameplayTag EffectTag, float& OutRemainingTime, float& OutTotalDuration)
{
    OutRemainingTime = 0.f;
    OutTotalDuration = 0.f;

    GB_CONDITION_CHECK(EffectTag.IsValid());

    const TArray<FActiveGameplayEffectHandle>& ActiveEffects = GetActiveEffectsWithAllTags(FGameplayTagContainer(EffectTag));
    const float Now = GetWorld()->GetTimeSeconds();

    for (const FActiveGameplayEffectHandle& Handle : ActiveEffects)
    {
        const FActiveGameplayEffect* ActiveEffect = GetActiveGameplayEffect(Handle);
        if (ActiveEffect == nullptr)
        {
            continue;
        }

        const float Duration = ActiveEffect->GetDuration();
        if (Duration == UGameplayEffect::INFINITE_DURATION)
        {
            continue;
        }

        OutTotalDuration = OutTotalDuration < Duration ? Duration : OutTotalDuration;

        const float RemainingTime = ActiveEffect->GetTimeRemaining(Now);
        OutRemainingTime = OutRemainingTime < RemainingTime ? RemainingTime : OutRemainingTime;
    }
}

void UGBAbilitySystemComponent::GetAbilityCoolTimeByAssetTag(const FGameplayTag AssetTag, float& OutRemainingTime, float& OutTotalDuration)
{
    OutRemainingTime = 0.f;
    OutTotalDuration = 0.f;

    GB_CONDITION_CHECK(AssetTag.IsValid());

    TArray<FGameplayAbilitySpec*> AbilitySpecs;
    GetAbilitySpecsByAssetTag(AssetTag, AbilitySpecs);

    for (FGameplayAbilitySpec* AbilitySpec : AbilitySpecs)
    {
        if (UGameplayAbility* Instance = AbilitySpec->GetPrimaryInstance())
        {
            AbilitySpec->Ability->GetCooldownTimeRemainingAndDuration(AbilitySpec->Handle, Instance->GetCurrentActorInfo(), OutRemainingTime, OutTotalDuration);
        }
        else
        {
            AbilitySpec->Ability->GetCooldownTimeRemainingAndDuration(AbilitySpec->Handle, AbilityActorInfo.Get(), OutRemainingTime, OutTotalDuration);
        }
    }
}

void UGBAbilitySystemComponent::InitAttributeStats(UGBAttributeData* AttributeInfo)
{
    GB_NULL_CHECK(AttributeInfo);

    const TArray<UClass*>& AttributeSetClasses = AttributeInfo->GetAttributeSetStaticClass();
    for (UClass* AttributeSetClass : AttributeSetClasses)
    {
        GetOrCreateAttributeSubobject(AttributeSetClass);
    }

    FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(InitAttributeStatGameplayEffectClass, 1.f, MakeEffectContext());
    GB_CONDITION_CHECK(SpecHandle.IsValid());

    AttributeInfo->SetAttributeInfo(SpecHandle);

    ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}
