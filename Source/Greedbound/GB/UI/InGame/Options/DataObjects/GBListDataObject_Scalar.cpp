// Fill out your copyright notice in the Description page of Project Settings.

#include "GBListDataObject_Scalar.h"
#include "UI/InGame/Options/GBOptionsDataInteractionHelper.h"

FCommonNumberFormattingOptions UGBListDataObject_Scalar::NoDecimal()
{
    FCommonNumberFormattingOptions Options;
    Options.MaximumFractionalDigits = 0;

    return Options;
}

FCommonNumberFormattingOptions UGBListDataObject_Scalar::WithDecimal(int32 NumFracDigit)
{
    FCommonNumberFormattingOptions Options;
    Options.MaximumFractionalDigits = NumFracDigit;

    return Options;
}

float UGBListDataObject_Scalar::GetCurrentValue() const
{
    if (DataDynamicGetter)
    {
        return FMath::GetMappedRangeValueClamped(
            OutputValueRange,
            DisplayValueRange,
            StringToFloat(DataDynamicGetter->GetValueAsString())
        );
    }

    return 0.f;
}

void UGBListDataObject_Scalar::SetCurrentValuieFromSlider(float InValue)
{
    if (DataDynamicSetter)
    {
        const float ClampedValue = FMath::GetMappedRangeValueClamped(
            DisplayValueRange,
            OutputValueRange,
            InValue
        );
        DataDynamicSetter->SetValueFromString(LexToString(ClampedValue));

        NotifyListDataModified(this);
    }
}

bool UGBListDataObject_Scalar::CanResetBackToDefaultValue() const
{
    if (HasDefaultValue() == false || DataDynamicGetter == nullptr)
    {
        return false;
    }

    const float DefaultValue = StringToFloat(GetDefaultValueAsString());
    const float CurrentValue = StringToFloat(DataDynamicGetter->GetValueAsString());

    return FMath::IsNearlyEqual(DefaultValue, CurrentValue, 0.01f) == false;
}

bool UGBListDataObject_Scalar::TryResetBackToDefaultValue()
{
    if (HasDefaultValue() == false || DataDynamicGetter == nullptr)
    {
        return false;
    }

    DataDynamicSetter->SetValueFromString(GetDefaultValueAsString());
    NotifyListDataModified(this, EGBOptionsListDataModifyReason::ResetToDefault);

    return true;
}

void UGBListDataObject_Scalar::OnEditDependencyDataModified(UGBListDataObject_Base* ModifiedDependencyData, EGBOptionsListDataModifyReason ModifyReason)
{
    NotifyListDataModified(this, EGBOptionsListDataModifyReason::DependencyModified);

    Super::NotifyListDataModified(ModifiedDependencyData, ModifyReason);
}

float UGBListDataObject_Scalar::StringToFloat(const FString& InString) const
{
    float OutConvertValue = 0.f;
    LexFromString(OutConvertValue, *InString);

    return OutConvertValue;
}
