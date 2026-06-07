// Fill out your copyright notice in the Description page of Project Settings.

#include "GBListEntryWidget_String.h"
#include "UI/InGame/Options/DataObjects/GBListDataObject_String.h"
#include "UI/InGame/Component/GBCommonRotator.h"
#include "UI/InGame/Component/GBCommonButtonBase.h"
#include "Define/GBDefine.h"
#include "CommonInputSubsystem.h"

void UGBListEntryWidget_String::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    CB_PrevOption->OnClicked().AddUObject(this, &UGBListEntryWidget_String::OnPreviousOptionButtonClicked);
    CB_NextOption->OnClicked().AddUObject(this, &UGBListEntryWidget_String::OnNextOptionButtonClicked);

    CR_AvaliableOptions->OnClicked().AddLambda([this]() {SelectThisEntryWidget();});
    CR_AvaliableOptions->OnRotatedEvent.AddUObject(this, &UGBListEntryWidget_String::OnRotatorValueChanged);
}

void UGBListEntryWidget_String::OnOwingListDataObjectSet(UGBListDataObject_Base* InOwningListDataObject)
{
    Super::OnOwingListDataObjectSet(InOwningListDataObject);

    CachedOwingStringDataObject = CastChecked<UGBListDataObject_String>(InOwningListDataObject);

    CR_AvaliableOptions->PopulateTextLabels(CachedOwingStringDataObject->GetAvailableOptionsTextList());
    CR_AvaliableOptions->SetSelectedOptionByText(CachedOwingStringDataObject->GetCurrentDisplayText());
}

void UGBListEntryWidget_String::OnOwningListDataObjectModified(UGBListDataObject_Base* InListDataObject, EGBOptionsListDataModifyReason ModifyReason)
{
    GB_NULL_CHECK(CachedOwingStringDataObject);

    CR_AvaliableOptions->SetSelectedOptionByText(CachedOwingStringDataObject->GetCurrentDisplayText());
}

void UGBListEntryWidget_String::OnToggledEditableState(bool bIsEditable)
{
    Super::OnToggledEditableState(bIsEditable);

    CB_PrevOption->SetIsEnabled(bIsEditable);
    CR_AvaliableOptions->SetIsEnabled(bIsEditable);
    CB_NextOption->SetIsEnabled(bIsEditable);
}

void UGBListEntryWidget_String::OnPreviousOptionButtonClicked()
{
    GB_NULL_CHECK(CachedOwingStringDataObject);

    CachedOwingStringDataObject->BackToPrevOption();

    SelectThisEntryWidget();
}

void UGBListEntryWidget_String::OnNextOptionButtonClicked()
{
    GB_NULL_CHECK(CachedOwingStringDataObject);

    CachedOwingStringDataObject->AdvanceToNextOption();

    SelectThisEntryWidget();
}

void UGBListEntryWidget_String::OnRotatorValueChanged(int32 InValue, bool bUserInitialized)
{
    GB_NULL_CHECK(CachedOwingStringDataObject);

    UCommonInputSubsystem* CommonInputSubsystem = GetInputSubsystem();
    GB_CONDITION_CHECK(CommonInputSubsystem != nullptr && bUserInitialized);

    if (CommonInputSubsystem->GetCurrentInputType() == ECommonInputType::Gamepad)
    {
        CachedOwingStringDataObject->OnRotatorInitiatedValueChange(CR_AvaliableOptions->GetSelectedText());
    }
}
