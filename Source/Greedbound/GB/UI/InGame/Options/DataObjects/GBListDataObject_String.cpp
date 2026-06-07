// Fill out your copyright notice in the Description page of Project Settings.

#include "GBListDataObject_String.h"
#include "UI/InGame/Options/GBOptionsDataInteractionHelper.h"
#include "Define/GBDefine.h"

void UGBListDataObject_String::OnDataObjectInitialized()
{
    if (AvailableOptionsStringList.IsEmpty() == false)
    {
        CurrentStringValue = AvailableOptionsStringList[0];
    }

    if (HasDefaultValue())
    {
        CurrentStringValue = GetDefaultValueAsString();
    }

    if (DataDynamicGetter)
    {
        if (DataDynamicGetter->GetValueAsString().IsEmpty() == false)
        {
            CurrentStringValue = DataDynamicGetter->GetValueAsString();
        }
    }

    if (TrySetDisplayTextFromStringValue(CurrentStringValue) == false)
    {
        CurrentDisplayText = FText::FromString(TEXT("INVALID OPTION"));
    }
}

bool UGBListDataObject_String::CanResetBackToDefaultValue() const
{
    return HasDefaultValue() && CurrentStringValue != GetDefaultValueAsString();
}

bool UGBListDataObject_String::TryResetBackToDefaultValue()
{
    if (CanResetBackToDefaultValue())
    {
        CurrentStringValue = GetDefaultValueAsString();
        TrySetDisplayTextFromStringValue(CurrentStringValue);

        if (DataDynamicSetter)
        {
            DataDynamicSetter->SetValueFromString(CurrentStringValue);
            NotifyListDataModified(this, EGBOptionsListDataModifyReason::ResetToDefault);

            return true;
        }
    }

    return false;
}

void UGBListDataObject_String::AddDynamicOption(const FString& InStringValue, const FText& InDisplayText)
{
    AvailableOptionsStringList.Add(InStringValue);
    AvailableOptionsTextList.Add(InDisplayText);
}

void UGBListDataObject_String::AdvanceToNextOption()
{
    if (AvailableOptionsStringList.IsEmpty() || AvailableOptionsTextList.IsEmpty())
    {
        return;
    }

    const int32 CurrentDisplayIndex = AvailableOptionsStringList.IndexOfByKey(CurrentStringValue);
    const int32 NextDisplayIndex = CurrentDisplayIndex + 1;

    if (AvailableOptionsStringList.IsValidIndex(NextDisplayIndex))
    {
        CurrentStringValue = AvailableOptionsStringList[NextDisplayIndex];
    }
    else
    {
        CurrentStringValue = AvailableOptionsStringList[0];
    }

    TrySetDisplayTextFromStringValue(CurrentStringValue);

    if (DataDynamicSetter)
    {
        DataDynamicSetter->SetValueFromString(CurrentStringValue);
        NotifyListDataModified(this);
    }
}

void UGBListDataObject_String::BackToPrevOption()
{
    if (AvailableOptionsStringList.IsEmpty() || AvailableOptionsTextList.IsEmpty())
    {
        return;
    }

    const int32 CurrentDisplayIndex = AvailableOptionsStringList.IndexOfByKey(CurrentStringValue);
    const int32 PrevDisplayIndex = CurrentDisplayIndex - 1;

    if (AvailableOptionsStringList.IsValidIndex(PrevDisplayIndex))
    {
        CurrentStringValue = AvailableOptionsStringList[PrevDisplayIndex];
    }
    else
    {
        CurrentStringValue = AvailableOptionsStringList.Last();
    }

    TrySetDisplayTextFromStringValue(CurrentStringValue);

    if (DataDynamicSetter)
    {
        DataDynamicSetter->SetValueFromString(CurrentStringValue);
        NotifyListDataModified(this);
    }
}

void UGBListDataObject_String::OnRotatorInitiatedValueChange(const FText& InNewSelectedText)
{
    const int32 FoundIndex = AvailableOptionsTextList.IndexOfByPredicate(
        [InNewSelectedText](const FText& AvailableText)->bool
        {
            return AvailableText.EqualTo(InNewSelectedText);
        }
    );

    if (FoundIndex != INDEX_NONE && AvailableOptionsTextList.IsValidIndex(FoundIndex))
    {
        CurrentDisplayText = InNewSelectedText;
        CurrentStringValue = AvailableOptionsStringList[FoundIndex];

        if (DataDynamicSetter)
        {
            DataDynamicSetter->SetValueFromString(CurrentStringValue);

            NotifyListDataModified(this);
        }
    }
}

bool UGBListDataObject_String::TrySetDisplayTextFromStringValue(const FString& InStringValue)
{
    const int32 CurrentFoundIndex = AvailableOptionsStringList.IndexOfByKey(InStringValue);

    if (AvailableOptionsStringList.IsValidIndex(CurrentFoundIndex))
    {
        CurrentDisplayText = AvailableOptionsTextList[CurrentFoundIndex];
        return true;
    }

    return false;
}

//=========================== StringBool ==================================

void UGBListDataObject_StringBool::OverrideTrueDisplayText(const FText& InNewTrueDisplayText)
{
    if (AvailableOptionsStringList.Contains(TrueString) == false)
    {
        AddDynamicOption(TrueString, InNewTrueDisplayText);
    }
}

void UGBListDataObject_StringBool::OverrideFalseDisplayText(const FText& InNewFalseDisplayText)
{
    if (AvailableOptionsStringList.Contains(FalseString) == false)
    {
        AddDynamicOption(FalseString, InNewFalseDisplayText);
    }
}

void UGBListDataObject_StringBool::SetTrueAsDefaultValue()
{
    SetDefaultValueFromString(TrueString);
}

void UGBListDataObject_StringBool::SetFalseAsDefaultValue()
{
    SetDefaultValueFromString(FalseString);
}

void UGBListDataObject_StringBool::OnDataObjectInitialized()
{
    TryInitBoolValues();

    Super::OnDataObjectInitialized();
}

void UGBListDataObject_StringBool::TryInitBoolValues()
{
    if (AvailableOptionsStringList.Contains(TrueString) == false)
    {
        AddDynamicOption(TrueString, FText::FromString(TEXT("ON")));
    }

    if (AvailableOptionsStringList.Contains(FalseString) == false)
    {
        AddDynamicOption(FalseString, FText::FromString(TEXT("OFF")));
    }
}

//=========================== StringInteger ==================================

void UGBListDataObject_StringInteger::AddIntegerOption(int32 InIntegerValue, const FText& InDisplayText)
{
    AddDynamicOption(LexToString(InIntegerValue), InDisplayText);
}

void UGBListDataObject_StringInteger::OnDataObjectInitialized()
{
    Super::OnDataObjectInitialized();

    if (TrySetDisplayTextFromStringValue(CurrentStringValue) == false)
    {
        CurrentDisplayText= FText::FromString(TEXT("Custom"));
    }
}

void UGBListDataObject_StringInteger::OnEditDependencyDataModified(UGBListDataObject_Base* ModifiedDependencyData, EGBOptionsListDataModifyReason ModifyReason)
{
    GB_NULL_CHECK(ModifiedDependencyData);

    if (DataDynamicGetter)
    {
        if (CurrentStringValue == DataDynamicGetter->GetValueAsString())
        {
            return;
        }

        CurrentStringValue = DataDynamicGetter->GetValueAsString();

        if (TrySetDisplayTextFromStringValue(CurrentStringValue) == false)
        {
            CurrentDisplayText = FText::FromString(TEXT("Custom"));
        }

        NotifyListDataModified(ModifiedDependencyData, EGBOptionsListDataModifyReason::DependencyModified);
    }

    Super::OnEditDependencyDataModified(ModifiedDependencyData, ModifyReason);
}
