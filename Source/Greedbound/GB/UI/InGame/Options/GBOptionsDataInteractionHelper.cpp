// Fill out your copyright notice in the Description page of Project Settings.

#include "GBOptionsDataInteractionHelper.h"
#include "ProjectSetting/GBGameUserSettings.h"

FGBOptionsDataInteractionHelper::FGBOptionsDataInteractionHelper(const FString& InSetterOrGetterFuncPath)
    : CachedDynamicFunctionPath(InSetterOrGetterFuncPath)
{
    CachedWeakGameUserSettings = UGBGameUserSettings::Get();
}

FString FGBOptionsDataInteractionHelper::GetValueAsString() const
{
    FString OutStringValue;
    PropertyPathHelpers::GetPropertyValueAsString(CachedWeakGameUserSettings.Get(), CachedDynamicFunctionPath, OutStringValue);

    return OutStringValue;
}

void FGBOptionsDataInteractionHelper::SetValueFromString(const FString& InStringValue)
{
    PropertyPathHelpers::SetPropertyValueFromString(CachedWeakGameUserSettings.Get(), CachedDynamicFunctionPath, InStringValue);
}
