// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GB/UI/InGame/Options/DataObjects/GBListDataObject_Base.h"
#include "GBListDataObject_Value.generated.h"

class FGBOptionsDataInteractionHelper;

UCLASS(Abstract)
class GREEDBOUND_API UGBListDataObject_Value : public UGBListDataObject_Base
{
	GENERATED_BODY()

public:
    void                                        SetDataDynamicGetter(const TSharedPtr<FGBOptionsDataInteractionHelper>& InDynamicGetter);
    void                                        SetDataDynamicSetter(const TSharedPtr<FGBOptionsDataInteractionHelper>& InDynamicSetter);

    void                                        SetDefaultValueFromString(const FString& InDefaultValue) { DefaultStringValuep = InDefaultValue; }

    virtual bool                                HasDefaultValue() const override final { return DefaultStringValuep.IsSet(); }

protected:
    FString                                     GetDefaultValueAsString() const;
	
protected:
    TSharedPtr<FGBOptionsDataInteractionHelper> DataDynamicGetter;
    TSharedPtr<FGBOptionsDataInteractionHelper> DataDynamicSetter;

private:
    TOptional<FString>                          DefaultStringValuep;
};
