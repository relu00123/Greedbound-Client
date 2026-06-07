// Fill out your copyright notice in the Description page of Project Settings.

#include "GBListEntryWidget_Base.h"
#include "UI/InGame/Options/DataObjects/GBListDataObject_Base.h"
#include "CommonTextBlock.h"
#include "Components/ListView.h"
#include "CommonInputSubsystem.h"
#include "Define/GBDefine.h"
void UGBListEntryWidget_Base::NativeOnListEntryWidgetHovered(bool bWasHovered)
{
    BP_OnListEntryWidgetHovered(bWasHovered, GetListItem() ? IsListItemSelected() : false);

    if (bWasHovered)
    {
        BP_OnToggleEntryWidgetHighlightState(true);
    }
    else
    {
        BP_OnToggleEntryWidgetHighlightState(GetListItem() && IsListItemSelected());
    }
}

void UGBListEntryWidget_Base::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

    OnOwingListDataObjectSet(CastChecked<UGBListDataObject_Base>(ListItemObject));
}

void UGBListEntryWidget_Base::NativeOnItemSelectionChanged(bool bIsSelected)
{
    IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);

    BP_OnToggleEntryWidgetHighlightState(bIsSelected);
}

void UGBListEntryWidget_Base::NativeOnEntryReleased()
{
    IUserObjectListEntry::NativeOnEntryReleased();

    NativeOnListEntryWidgetHovered(false);
}

FReply UGBListEntryWidget_Base::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
    UCommonInputSubsystem* CommonInputSubSystem = GetInputSubsystem();
    if (CommonInputSubSystem && CommonInputSubSystem->GetCurrentInputType() == ECommonInputType::Gamepad)
    {
        if (UWidget* WidgetToFocus = BP_GetWidgetToFocusForGamepad())
        {
            if (TSharedPtr<SWidget> SlateWidgetToFocus = WidgetToFocus->GetCachedWidget())
            {
                return FReply::Handled().SetUserFocus(SlateWidgetToFocus.ToSharedRef());
            }
        }
    }

    return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UGBListEntryWidget_Base::OnOwingListDataObjectSet(UGBListDataObject_Base* InOwningListDataObject)
{
    GB_NULL_CHECK(InOwningListDataObject);

    if (CTB_SettingDisplayName)
    {
        CTB_SettingDisplayName->SetText(InOwningListDataObject->GetDataDisplayName());
    }

    if (InOwningListDataObject->OnListDataModified.IsBoundToObject(this) == false)
    {
        InOwningListDataObject->OnListDataModified.AddUObject(this, &UGBListEntryWidget_Base::OnOwningListDataObjectModified);
    }

    if (InOwningListDataObject->OnDependencyDataModified.IsBoundToObject(this) == false)
    {
        InOwningListDataObject->OnDependencyDataModified.AddUObject(this, &UGBListEntryWidget_Base::OnOwningDependencyDataObjectModified);
    }

    OnToggledEditableState(InOwningListDataObject->IsDataCurrentlyEditable());

    CachedOwningDataObject = InOwningListDataObject;
}

void UGBListEntryWidget_Base::OnOwningDependencyDataObjectModified(UGBListDataObject_Base* InDependencyDataObject, EGBOptionsListDataModifyReason ModifyReason)
{
    GB_NULL_CHECK(InDependencyDataObject);

    if (CachedOwningDataObject)
    {
        OnToggledEditableState(CachedOwningDataObject->IsDataCurrentlyEditable());
    }
}

void UGBListEntryWidget_Base::OnToggledEditableState(bool bIsEditable)
{
    if (CTB_SettingDisplayName)
    {
        CTB_SettingDisplayName->SetIsEnabled(bIsEditable);
    }
}

void UGBListEntryWidget_Base::SelectThisEntryWidget()
{
    CastChecked<UListView>(GetOwningListView())->SetSelectedItem(GetListItem());
}
