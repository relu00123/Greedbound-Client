// Fill out your copyright notice in the Description page of Project Settings.

#include "GBCharacterData.h"
#include "Define/GBTags.h"
#include "Define/GBDefine.h"
#include "AbilitySystem/Attribute/GBHealthAttributeSet.h"
#include "AbilitySystem/Attribute/GBSpeedAttributeSet.h"
#include "AbilitySystem/Attribute/GBCombatAttributeSet.h"

const TArray<UClass*> UGBAttributeData::GetAttributeSetStaticClass() const
{
    TArray<UClass*> StaticClasses;

    // Health
    if (AttributeFlags & static_cast<int32>(EGBAttributeFlags::Health))
    {
        StaticClasses.Add(UGBHealthAttributeSet::StaticClass());
    }

    // Speed
    if (AttributeFlags & static_cast<int32>(EGBAttributeFlags::Speed))
    {
        StaticClasses.Add(UGBSpeedAttributeSet::StaticClass());
    }

    // Combat
    if (AttributeFlags & static_cast<int32>(EGBAttributeFlags::Combat))
    {
        StaticClasses.Add(UGBCombatAttributeSet::StaticClass());
    }

    return StaticClasses;
}

void UGBAttributeData::SetAttributeInfo(FGameplayEffectSpecHandle& SpecHandle) const
{
    // Health
    if (AttributeFlags & static_cast<int32>(EGBAttributeFlags::Health))
    {
        for (TPair<FGameplayTag, float> Data : HealthAttributeData)
        {
            SpecHandle.Data->SetSetByCallerMagnitude(Data.Key, Data.Value);
        }
    }

    // Speed
    if (AttributeFlags & static_cast<int32>(EGBAttributeFlags::Speed))
    {
        for (TPair<FGameplayTag, float> Data : SpeedAttributeData)
        {
            SpecHandle.Data->SetSetByCallerMagnitude(Data.Key, Data.Value);
        }
    }

    // Combat
    if (AttributeFlags & static_cast<int32>(EGBAttributeFlags::Combat))
    {
        for (TPair<FGameplayTag, float> Data : CombatAttributeData)
        {
            SpecHandle.Data->SetSetByCallerMagnitude(Data.Key, Data.Value);
        }
    }
}

#if WITH_EDITOR
void UGBAttributeData::PostLoad()
{
    UpdateAttributeFlags();

    Super::PostLoad();
}

void UGBAttributeData::UpdateAttributeFlags()
{
    bHasHealth = (AttributeFlags & EGBAttributeFlags::Health) != 0;
    if (bHasHealth)
    {
        const FGameplayTagContainer& ChildTag = UGameplayTagsManager::Get().RequestGameplayTagChildren(GBTag::Attribute_Health);;
        for (const FGameplayTag& Tag : ChildTag)
        {
            if (HealthAttributeData.Contains(Tag) == false)
            {
                HealthAttributeData.Add(Tag, 0.f);
            }
        }
    }
    else
    {
        HealthAttributeData.Empty();
    }

    bHasSpeed = (AttributeFlags & EGBAttributeFlags::Speed) != 0;
    if (bHasSpeed)    
    {
        const FGameplayTagContainer& ChildTag = UGameplayTagsManager::Get().RequestGameplayTagChildren(GBTag::Attribute_Speed);
        for (const FGameplayTag& Tag : ChildTag)
        {
            if (SpeedAttributeData.Contains(Tag) == false)
            {
                SpeedAttributeData.Add(Tag, 0.f);
            }
        }
    }
    else
    {
        SpeedAttributeData.Empty();
    }

    bHasCombat = (AttributeFlags & EGBAttributeFlags::Combat) != 0;
    if (bHasCombat)
    {
        const FGameplayTagContainer& ChildTag = UGameplayTagsManager::Get().RequestGameplayTagChildren(GBTag::Attribute_Combat);
        for (const FGameplayTag& Tag : ChildTag)
        {
            if (CombatAttributeData.Contains(Tag) == false)
            {
                CombatAttributeData.Add(Tag, 0.f);
            }
        }
    }
    else
    {
        CombatAttributeData.Empty();
    }
}

void UGBAttributeData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UGBAttributeData, AttributeFlags))
    {
        UpdateAttributeFlags();
    }

    Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif // WITH_EDITOR


FPrimaryAssetId UGBCharacterData::GetPrimaryAssetId() const
{
    FName AssetName = NAME_None;
    FString LastPart;

    if (CharacterTag.IsValid())
    {
        FString AssetNameTagStr = CharacterTag.ToString();
        AssetNameTagStr.Split(TEXT("."), nullptr, &LastPart, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
        AssetName = FName(*LastPart);
    }

    return FPrimaryAssetId(FName("CharacterData"), AssetName);
}

TArray<FGameplayAbilitySpec> UGBCharacterData::GetAbilitySpecs()
{
    TArray<FGameplayAbilitySpec> AbilitySpecs;
    for (TSoftObjectPtr<UGBCharacterAbilityData>& AbiliyInfo : AbilityInfos)
    {
        if (AbiliyInfo.IsNull())
        {
            GB_LOG(TEXT("CharacterAbilityData Softobjectptr is Invalid!"));
            continue;
        }

        if (AbiliyInfo.IsValid() == false)
        {
            AbiliyInfo.LoadSynchronous();
        }

        UGBCharacterAbilityData* AbilityData = AbiliyInfo.Get();
        GB_NULL_CHECK_WITH_RETURN(AbilityData, AbilitySpecs);

        for (TSubclassOf<UGameplayAbility> AbilityClass : AbilityData->Abilities)
        {
            if (AbilityClass)
            {
                AbilitySpecs.Add(FGameplayAbilitySpec(AbilityClass));
            }
        }
    }

    return AbilitySpecs;
}
