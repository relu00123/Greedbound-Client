// Fill out your copyright notice in the Description page of Project Settings.

#include "GBListEntryWidget_Scalar.h"
#include "AnalogSlider.h"
#include "UI/InGame/Options/DataObjects/GBListDataObject_Scalar.h"
#include "Define/GBDefine.h"

void UGBListEntryWidget_Scalar::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    AS_SettingSlider->OnValueChanged.AddUniqueDynamic(this, &UGBListEntryWidget_Scalar::OnSliderValueChanged);
    AS_SettingSlider->OnMouseCaptureBegin.AddUniqueDynamic(this, &UGBListEntryWidget_Scalar::OnSliderMouseCaptureBegin);
}

void UGBListEntryWidget_Scalar::OnOwingListDataObjectSet(UGBListDataObject_Base* InOwningListDataObject)
{
    Super::OnOwingListDataObjectSet(InOwningListDataObject);

    CachedOwningScalarDataObject = CastChecked<UGBListDataObject_Scalar>(InOwningListDataObject);

    CNTB_SettingValue->SetNumericType(CachedOwningScalarDataObject->GetDisplayNumericType());
    CNTB_SettingValue->FormattingSpecification = CachedOwningScalarDataObject->GetNumberFormattingOptions();
    CNTB_SettingValue->SetCurrentValue(CachedOwningScalarDataObject->GetCurrentValue());

    AS_SettingSlider->SetMinValue(CachedOwningScalarDataObject->GetDisplayValueRange().GetLowerBoundValue());
    AS_SettingSlider->SetMaxValue(CachedOwningScalarDataObject->GetDisplayValueRange().GetUpperBoundValue());
    AS_SettingSlider->SetStepSize(CachedOwningScalarDataObject->GetSliderStepSize());
    AS_SettingSlider->SetValue(CachedOwningScalarDataObject->GetCurrentValue());
}

void UGBListEntryWidget_Scalar::OnOwningListDataObjectModified(UGBListDataObject_Base* InListDataObject, EGBOptionsListDataModifyReason ModifyReason)
{
    GB_NULL_CHECK(CachedOwningScalarDataObject);

    CNTB_SettingValue->SetCurrentValue(CachedOwningScalarDataObject->GetCurrentValue());
    AS_SettingSlider->SetValue(CachedOwningScalarDataObject->GetCurrentValue());
}

void UGBListEntryWidget_Scalar::OnSliderValueChanged(float InValue)
{
    GB_NULL_CHECK(CachedOwningScalarDataObject);

    CachedOwningScalarDataObject->SetCurrentValuieFromSlider(InValue);
}

void UGBListEntryWidget_Scalar::OnSliderMouseCaptureBegin()
{
    SelectThisEntryWidget();
}
