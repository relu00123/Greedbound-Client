// Fill out your copyright notice in the Description page of Project Settings.

#include "GBAssetLoadSubsystem.h"
#include "Data/GBAssetManager.h"
#include "Data/GBItemData.h"
#include "Define/GBDefine.h"
#include "Define/GBTags.h"
#include "Define/Character/GBClassType.h"
#include "Data/GBCharacterData.h"
#include "Components/GBAbilitySystemComponent.h"

UGBAssetLoadSubsystem* UGBAssetLoadSubsystem::Get(const UObject* WorldContextObject)
{
    if (GEngine)
    {
        UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
        return UGameInstance::GetSubsystem<UGBAssetLoadSubsystem>(World->GetGameInstance());
    }

    return nullptr;
}

void UGBAssetLoadSubsystem::LoadCharacterDataByAssetId(const FPrimaryAssetId& AssetToLoad, UGBAbilitySystemComponent* GBASC)
{
    GB_NULL_CHECK(GBASC);

    UGBAssetManager& AM = UGBAssetManager::Get();
    TSharedPtr<FStreamableHandle> StreamableHandle = AM.LoadPrimaryAsset(AssetToLoad);

    UGBCharacterData* AbilityData = nullptr;
    if (StreamableHandle.IsValid())
    {
        StreamableHandle->WaitUntilComplete();
        AbilityData = StreamableHandle->GetLoadedAsset<UGBCharacterData>();
        StreamableHandle->ReleaseHandle();
    }
    else
    {
        AbilityData = AM.GetPrimaryAssetObject<UGBCharacterData>(AssetToLoad);
    }

    GB_NULL_CHECK(AbilityData);

    const TArray<FGameplayAbilitySpec>& Specs = AbilityData->GetAbilitySpecs();
    GB_CONDITION_CHECK(Specs.IsEmpty() == false);

    for (const FGameplayAbilitySpec& Spec : Specs)
    {
        GBASC->GiveAbility(Spec);
    }

    GB_CONDITION_CHECK(AbilityData->AttributeInfo.IsNull() == false);

    if (AbilityData->AttributeInfo.IsValid() == false)
    {
        AbilityData->AttributeInfo.LoadSynchronous();
    }

    UGBAttributeData* AttributeData = AbilityData->AttributeInfo.Get();
    GB_NULL_CHECK(AttributeData);

    GBASC->InitAttributeStats(AttributeData);
    if (GBASC->GetOwnerRole() == ENetRole::ROLE_Authority)
    {
        GBASC->AddLooseGameplayTag(AbilityData->CharacterTag);
        GBASC->AddReplicatedLooseGameplayTag(AbilityData->CharacterTag);
    }
}

void UGBAssetLoadSubsystem::LoadCharacterDataByTag(const FGameplayTag& AssetNameTag, UGBAbilitySystemComponent* GBASC)
{
    GB_CONDITION_CHECK(AssetNameTag.IsValid());

    FName AssetType = NAME_None;
    FName AssetName = NAME_None;
    FString LastPart;

    FString AssetTypeTagStr = GBTag::AssetType_CharacterData.GetTag().ToString();
    AssetTypeTagStr.Split(TEXT("."), nullptr, &LastPart, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
    AssetType = FName(*LastPart);

    FString AssetNameTagStr = AssetNameTag.ToString();
    AssetNameTagStr.Split(TEXT("."), nullptr, &LastPart, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
    AssetName = FName(*LastPart);

    LoadCharacterDataByAssetId(FPrimaryAssetId(AssetType, AssetName), GBASC);
}

void UGBAssetLoadSubsystem::LoadCharacterDataByClassType(const EGBCharacterClassType ClassType, UGBAbilitySystemComponent* GBASC)
{
    FGameplayTag AssetNameTag;
    switch (ClassType)
    {
    case EGBCharacterClassType::Archer:
        AssetNameTag = GBTag::Character_Player_Archer;
        break;
    case EGBCharacterClassType::Cleric:
        AssetNameTag = GBTag::Character_Player_Cleric;
        break;
    case EGBCharacterClassType::Knight:
        AssetNameTag = GBTag::Character_Player_Knight;
        break;
    case EGBCharacterClassType::Mage:
        AssetNameTag = GBTag::Character_Player_Mage;
        break;
    }

    GB_CONDITION_CHECK(AssetNameTag.IsValid());

    LoadCharacterDataByTag(AssetNameTag, GBASC);
}

void UGBAssetLoadSubsystem::LoadItemDataByAssetId(const FPrimaryAssetId& AssetId, FGBOnItemLoaded OnLoaded)
{
    UGBBaseItemData* ItemDataAsset = nullptr;

    UGBAssetManager& AM = UGBAssetManager::Get();

    AM.LoadPrimaryAsset(AssetId, TArray<FName>{},
        FStreamableDelegate::CreateLambda(
            [AssetId, OnLoaded, ItemDataAsset, &AM]() mutable
            {
                UObject* LoadedObject = AM.GetPrimaryAssetObject(AssetId);
                ItemDataAsset = Cast<UGBBaseItemData>(LoadedObject);

                const bool bSuccess = (ItemDataAsset != nullptr);
                OnLoaded.ExecuteIfBound(bSuccess, ItemDataAsset);
            }
        )
    );
}
