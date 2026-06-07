// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GBCharacterData.generated.h"

class UGameplayAbility;
class UAbilitySystemComponent;
struct FGameplayEffectSpecHandle;
struct FGameplayAbilitySpec;

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum EGBAttributeFlags : uint8
{
    None    = 0         UMETA(Hidden),
    Health  = 1 << 0    UMETA(DisplayName = "Health"),
    Speed   = 1 << 1    UMETA(DisplayName = "Speed"),
    Combat  = 1 << 2    UMETA(DisplayName = "Combat"),
    Count   = 1 << 3    UMETA(Hidden),
};

UCLASS(BlueprintType)
class UGBAttributeData : public UDataAsset
{
    GENERATED_BODY()

public:
    const TArray<UClass*>       GetAttributeSetStaticClass() const;
    void                        SetAttributeInfo(FGameplayEffectSpecHandle& SpecHandle) const;

#if WITH_EDITOR
    virtual void                PostLoad();
    virtual void                PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);

private:
    void                        UpdateAttributeFlags();
#endif // WITH_EDITOR

public:
    UPROPERTY(EditDefaultsOnly, meta = (Bitmask, BitmaskEnum = "/Script/Greedbound.EGBAttributeFlags"))
    int32                       AttributeFlags;

    UPROPERTY(Transient, meta = (HideInDetailPanel))
    bool                        bHasHealth;

    UPROPERTY(EditDefaultsOnly, meta = (ForceInlineRow, EditConditionHides, EditCondition = "bHasHealth", Categories = "Data.Attribute"))
    TMap<FGameplayTag, float>   HealthAttributeData;

    UPROPERTY(Transient, meta = (HideInDetailPanel))
    bool                        bHasSpeed;

    UPROPERTY(EditDefaultsOnly, meta = (ForceInlineRow, EditConditionHides, EditCondition = "bHasSpeed", Categories = "Data.Attribute"))
    TMap<FGameplayTag, float>   SpeedAttributeData;

    UPROPERTY(Transient, meta = (HideInDetailPanel))
    bool                        bHasCombat;

    UPROPERTY(EditDefaultsOnly, meta = (ForceInlineRow, EditConditionHides, EditCondition = "bHasCombat", Categories = "Data.Attribute"))
    TMap<FGameplayTag, float>   CombatAttributeData;
};

UCLASS(BlueprintType)
class GREEDBOUND_API UGBCharacterAbilityData : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UGameplayAbility>>	Abilities;
};

/*
 * 모든 Character 관련 초기 세팅 Data는 여기에 있어야 함
 */
UCLASS(BlueprintType)
class GREEDBOUND_API UGBCharacterData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    virtual FPrimaryAssetId                         GetPrimaryAssetId() const override;
    TArray<FGameplayAbilitySpec>                    GetAbilitySpecs();

public:
    UPROPERTY(EditDefaultsOnly)
    TArray<TSoftObjectPtr<UGBCharacterAbilityData>> AbilityInfos;

    UPROPERTY(EditDefaultsOnly)
    TSoftObjectPtr<UGBAttributeData>                AttributeInfo;
    
    // 해당 태그를 설정하면 PrimaryAssetId로 사용됨
    UPROPERTY(EditDefaultsOnly, meta = (Categories = "Character"))
    FGameplayTag                                    CharacterTag;
};
