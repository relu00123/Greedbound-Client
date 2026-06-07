// Fill out your copyright notice in the Description page of Project Settings.

#include "GBInputData.h"

FGBInputAction::FGBInputAction(UInputAction* NewInputAction, const FGameplayTag& NewAssetTag, const EGBTriggerType NewTriggerType)
    : InputAction(NewInputAction)
    , AssetTag(NewAssetTag)
    , TriggerType(NewTriggerType)
{
}

UGBInputData::UGBInputData()
{
}

EGBTriggerType UGBInputData::GetTriggerTypeByTag(const FGameplayTag& AssetTag) const
{
    for (const FGBInputAction& Data : AbilityInputActions)
    {
        if (Data.AssetTag == AssetTag)
        {
            return Data.TriggerType;
        }
    }

    return EGBTriggerType::None;
}
