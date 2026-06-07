// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GB/UI/InGame/Options/DataObjects/GBListDataObject_Value.h"
#include "CommonNumericTextBlock.h"
#include "GBListDataObject_Scalar.generated.h"

UCLASS()
class GREEDBOUND_API UGBListDataObject_Scalar : public UGBListDataObject_Value
{
	GENERATED_BODY()

public:
    LIST_DATA_ACCESSOR(TRange<float>, DisplayValueRange)
    LIST_DATA_ACCESSOR(TRange<float>, OutputValueRange)
    LIST_DATA_ACCESSOR(float, SliderStepSize)
    LIST_DATA_ACCESSOR(ECommonNumericType, DisplayNumericType)
    LIST_DATA_ACCESSOR(FCommonNumberFormattingOptions, NumberFormattingOptions)

    static FCommonNumberFormattingOptions   NoDecimal();
    static FCommonNumberFormattingOptions   WithDecimal(int32 NumFracDigit);

    float                                   GetCurrentValue() const;
    void                                    SetCurrentValuieFromSlider(float InValue);

private:
    virtual bool                            CanResetBackToDefaultValue() const override final;
    virtual bool                            TryResetBackToDefaultValue() override final;
    virtual void                            OnEditDependencyDataModified(UGBListDataObject_Base* ModifiedDependencyData, EGBOptionsListDataModifyReason ModifyReason) override final;

    float                                   StringToFloat(const FString& InString) const;
	
private:
    TRange<float>                           DisplayValueRange   = TRange<float>(0.f, 1.f);
    TRange<float>                           OutputValueRange    = TRange<float>(0.f, 1.f);
    float                                   SliderStepSize      = 0.1f;
    ECommonNumericType                      DisplayNumericType  = ECommonNumericType::Number;
    FCommonNumberFormattingOptions          NumberFormattingOptions;
};
