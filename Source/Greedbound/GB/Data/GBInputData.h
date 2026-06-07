// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GBInputData.generated.h"

class UInputAction;

UENUM(BlueprintType)
enum class EGBTriggerType : uint8
{
    None,
    Toggle,
    Hold,
    Trigger,
};

USTRUCT(BlueprintType)
struct FGBInputAction
{
    GENERATED_BODY()

public:
    FGBInputAction() = default;
    FGBInputAction(UInputAction* NewInputAction, const FGameplayTag& NewAbilityAssetTag, const EGBTriggerType  NewTriggerType);

public:
    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UInputAction>    InputAction;

    UPROPERTY(EditDefaultsOnly, meta = (Categories = "Trigger"))
    FGameplayTag                AssetTag;

    UPROPERTY(EditDefaultsOnly)
    EGBTriggerType              TriggerType = EGBTriggerType::None;
};

UCLASS(BlueprintType)
class GREEDBOUND_API UGBInputData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UGBInputData();
    
    UFUNCTION(BlueprintCallable)
    virtual FPrimaryAssetId GetPrimaryAssetId() const override { return FPrimaryAssetId("InputData", GetFName()); }

    EGBTriggerType          GetTriggerTypeByTag(const FGameplayTag& AssetTag) const;

public:
    UPROPERTY(EditAnywhere)
    TArray<FGBInputAction>  AbilityInputActions;
};
