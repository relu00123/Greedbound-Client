// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GBAbilitySystemComponent.generated.h"

class UGBInputData;
class UGBAttributeData;
struct FGameplayTagContainer;
enum class EGBTriggerType : uint8;

UCLASS()
class GREEDBOUND_API UGBAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:
    UGBAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    void                            AbilityTagTriggered(FGameplayTag AssetTag);
    void                            AbilityTagReleased(FGameplayTag AssetTag);
    void                            AbilityTagToggled(FGameplayTag AssetTag);
    void                            SetInputData(UGBInputData* NewInputData);

    const FGameplayTagContainer&    GetActivationOwnedTags(UGameplayAbility* Ability) const;
    EGBTriggerType                  GetAbilityTriggerTypeByAssetTag(FGameplayTag AssetTag) const;
    void                            GetAbilitySpecsByAssetTag(FGameplayTag AssetTag, TArray<FGameplayAbilitySpec*>& Outlist);
    void                            GetGameplayEffectDurationByAssetTag(FGameplayTag EffectTag, float& OutRemainingTime, float& OutTotalDuration);
    void                            GetAbilityCoolTimeByAssetTag(FGameplayTag AssetTag, float& OutRemainingTime, float& OutTotalDuration);

    void                            InitAttributeStats(UGBAttributeData* AttributeInfo);
    
private:
    UPROPERTY()
    TSubclassOf<UGameplayEffect>    InitAttributeStatGameplayEffectClass;

    UPROPERTY()
    TObjectPtr<UGBInputData>        InputData;
};
