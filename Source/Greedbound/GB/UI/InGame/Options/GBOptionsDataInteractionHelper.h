// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PropertyPathHelpers.h"

class UGBGameUserSettings;

class GREEDBOUND_API FGBOptionsDataInteractionHelper
{
public:
    FGBOptionsDataInteractionHelper(const FString& InSetterOrGetterFuncPath);

    FString                             GetValueAsString() const;
    void                                SetValueFromString(const FString& InStringValue);

private:
    FCachedPropertyPath                 CachedDynamicFunctionPath;
    TWeakObjectPtr<UGBGameUserSettings> CachedWeakGameUserSettings;
};
