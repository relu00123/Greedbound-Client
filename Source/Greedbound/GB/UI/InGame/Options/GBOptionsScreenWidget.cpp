// Fill out your copyright notice in the Description page of Project Settings.

#include "GBOptionsScreenWidget.h"
#include "Input/CommonUIInputTypes.h"
#include "ProjectSetting/GBGameUserSettings.h"
#include "Subsystem/UI/GBUISubsystem.h"
#include "Define/GBDefine.h"
#include "UI/InGame/Component/GBTabListWidgetBase.h"
#include "UI/InGame/Component/GBCommonListView.h"
#include "UI/InGame/Component/GBCommonButtonBase.h"
#include "UI/InGame/Options/GBOptionsDataRegistry.h"
#include "UI/InGame/Options/DataObjects/GBListDataObject_Collection.h"
#include "UI/InGame/Options/ListEntries/GBListEntryWidget_Base.h"
#include "UI/InGame/Options/GBOptionsDetailsViewWidget.h"

void UGBOptionsScreenWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (ResetAction.IsNull() == false)
    {
        ResetActionHandle = RegisterUIActionBinding(
            FBindUIActionArgs(
                ResetAction,
                true,
                FSimpleDelegate::CreateUObject(this, &UGBOptionsScreenWidget::OnResetBoundActionTriggered)
            )
        );
    }

    CTL_OptionsTabListWidget->OnTabSelected.AddUniqueDynamic(this, &UGBOptionsScreenWidget::OnOptionsTabSelected);

    CLV_OptionList->OnItemIsHoveredChanged().AddUObject(this, &UGBOptionsScreenWidget::OnListViewItemHovered);
    CLV_OptionList->OnItemSelectionChanged().AddUObject(this, &UGBOptionsScreenWidget::OnListViewItemSelected);
}

void UGBOptionsScreenWidget::NativeOnActivated()
{
    Super::NativeOnActivated();

    const TArray<UGBListDataObject_Collection*>& RegisteredTabCollections = GetOrCreateDataRegistry()->GetRegisteredOptionsTabCollections();
    for (UGBListDataObject_Collection* TabCollection : RegisteredTabCollections)
    {
        if (TabCollection)
        {
            const FName TabId = TabCollection->GetDataID();
            if (CTL_OptionsTabListWidget->GetTabButtonBaseByID(TabId))
            {
                continue;
            }

            CTL_OptionsTabListWidget->RequestRegisterTab(TabId, TabCollection->GetDataDisplayName());
        }
    }
}

void UGBOptionsScreenWidget::NativeOnDeactivated()
{
    Super::NativeOnDeactivated();

    UGBGameUserSettings::Get()->ApplySettings(true);
}

UWidget* UGBOptionsScreenWidget::NativeGetDesiredFocusTarget() const
{
    UGBUISubsystem::Get(this)->SetHUDVisible(false);

    if (UObject* SelectedObject = CLV_OptionList->GetSelectedItem())
    {
        if (UUserWidget* SelectedEntryWidget = CLV_OptionList->GetEntryWidgetFromItem(SelectedObject))
        {
            return SelectedEntryWidget;
        }
    }

    if (CLV_OptionList && CLV_OptionList->GetNumItems() > 0)
    {
        return CLV_OptionList;
    }

    if (CTL_OptionsTabListWidget)
    {
        return CTL_OptionsTabListWidget;
    }

    return Super::NativeGetDesiredFocusTarget();
}

void UGBOptionsScreenWidget::OnResetBoundActionTriggered()
{
    GB_CONDITION_CHECK(ResetableDataList.IsEmpty() == false);

    UCommonButtonBase* SelectedTabButton = CTL_OptionsTabListWidget->GetTabButtonBaseByID(CTL_OptionsTabListWidget->GetActiveTab());
    const FString SelectedButtonName = CastChecked<UGBCommonButtonBase>(SelectedTabButton)->GetButtonDisplayText().ToString();

    UGBUISubsystem::Get(this)->PushConfirmScreenToModalStackAynsc(
        EGBConfirmScreenType::YesNo,
        FText::FromString(TEXT("Reset")),
        FText::FromString(TEXT("Are you sure you want to reset all the setting under the ") + SelectedButtonName + TEXT(" tab")),
        [this](EGBConfirmScreenButtonType ClickButtonType) 
        {
            if (ClickButtonType != EGBConfirmScreenButtonType::Confirmed)
            {
                return;
            }

            bIsResettingData = true;
            bool bHasDataFailedToReset = false;

            for (UGBListDataObject_Base* DataToReset : ResetableDataList)
            {
                if (DataToReset == nullptr)
                {
                    continue;
                }

                if (DataToReset->TryResetBackToDefaultValue() == false)
                {
                    bHasDataFailedToReset = true;
                }
            }

            if (bHasDataFailedToReset == false)
            {
                ResetableDataList.Empty();
                RemoveActionBinding(ResetActionHandle);
            }

            bIsResettingData = false;
        }
    );
}

UGBOptionsDataRegistry* UGBOptionsScreenWidget::GetOrCreateDataRegistry()
{
    if (OptionsDataRegistry == nullptr)
    {
        OptionsDataRegistry = NewObject<UGBOptionsDataRegistry>();
        OptionsDataRegistry->InitOptionsDataRegistry(GetOwningLocalPlayer());
    }

    GB_NULL_CHECK_WITH_RETURN(OptionsDataRegistry, nullptr);

    return OptionsDataRegistry;
}

void UGBOptionsScreenWidget::OnOptionsTabSelected(FName TabId)
{
    DetailsView_ListEntryInfo->ClearDetailsViewInfo();

    TArray<UGBListDataObject_Base*> FoundListSourceItems = GetOrCreateDataRegistry()->GetListSourceItemBySelectedTabID(TabId);
    CLV_OptionList->SetListItems(FoundListSourceItems);
    CLV_OptionList->RequestRefresh();

    if (CLV_OptionList->GetNumItems() != 0)
    {
        CLV_OptionList->NavigateToIndex(0);
        CLV_OptionList->SetSelectedIndex(0);
    }

    for (UGBListDataObject_Base* FoundListSourceItem : FoundListSourceItems)
    {
        if (FoundListSourceItem == nullptr)
        {
            continue;
        }

        if (FoundListSourceItem->OnListDataModified.IsBoundToObject(this) == false)
        {
            FoundListSourceItem->OnListDataModified.AddUObject(this, &UGBOptionsScreenWidget::OnListViewListDataModified);
        }

        if (FoundListSourceItem->CanResetBackToDefaultValue())
        {
            ResetableDataList.AddUnique(FoundListSourceItem);
        }

    }

    if (ResetableDataList.IsEmpty())
    {
        RemoveActionBinding(ResetActionHandle);
    }
    else
    {
        if (GetActionBindings().Contains(ResetActionHandle) == false)
        {
            AddActionBinding(ResetActionHandle);
        }
    }
}

void UGBOptionsScreenWidget::OnListViewItemHovered(UObject* InHoveredItem, bool bWasHovered)
{
    GB_NULL_CHECK(InHoveredItem);

    UGBListEntryWidget_Base* HoveredEntryWidget = CLV_OptionList->GetEntryWidgetFromItem<UGBListEntryWidget_Base>(InHoveredItem);
    GB_NULL_CHECK(HoveredEntryWidget);

    HoveredEntryWidget->NativeOnListEntryWidgetHovered(bWasHovered);

    if (bWasHovered)
    {
        DetailsView_ListEntryInfo->UpdateDetailsViewInfo(
            CastChecked<UGBListDataObject_Base>(InHoveredItem),
            TryGetEntryWidgetClassName(InHoveredItem));
    }
    else
    {
        UGBListDataObject_Base* SelectedItem = CLV_OptionList->GetSelectedItem<UGBListDataObject_Base>();
        if (SelectedItem)
        {
            DetailsView_ListEntryInfo->UpdateDetailsViewInfo(
                SelectedItem,
                TryGetEntryWidgetClassName(SelectedItem));
        }
    }
}

void UGBOptionsScreenWidget::OnListViewItemSelected(UObject* InSelectedItem)
{
    GB_NULL_CHECK(InSelectedItem);

    DetailsView_ListEntryInfo->UpdateDetailsViewInfo(
        CastChecked<UGBListDataObject_Base>(InSelectedItem),
        TryGetEntryWidgetClassName(InSelectedItem));
}

FString UGBOptionsScreenWidget::TryGetEntryWidgetClassName(UObject* InOwningListItem) const
{
    UUserWidget* FoundEntryWidget = CLV_OptionList->GetEntryWidgetFromItem(InOwningListItem);
    return FoundEntryWidget == nullptr ? TEXT("Entry Widget Not Valid") : FoundEntryWidget->GetClass()->GetName();
}

void UGBOptionsScreenWidget::OnListViewListDataModified(UGBListDataObject_Base* ModifiedData, EGBOptionsListDataModifyReason ModifiedReason)
{
    GB_CONDITION_CHECK(ModifiedData != nullptr && bIsResettingData == false);

    if (ModifiedData->CanResetBackToDefaultValue())
    {
        ResetableDataList.AddUnique(ModifiedData);

        if (GetActionBindings().Contains(ResetActionHandle) == false)
        {
            AddActionBinding(ResetActionHandle);
        }
    }
    else
    {
        if (ResetableDataList.Contains(ModifiedData))
        {
            ResetableDataList.Remove(ModifiedData);
        }
    }

    if (ResetableDataList.IsEmpty())
    {
        RemoveActionBinding(ResetActionHandle);
    }
}
