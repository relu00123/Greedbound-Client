// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InGame/Inventory/ListEntries/GBListEntryWidget_Item.h"
#include "Define/GBDefine.h"
#include "UI/InGame/Component/GBCommonButtonBase.h"
#include "UI/InGame/Options/DataObjects/GBListDataObject_Base.h"
#include "UI/InGame/Inventory/DataObjects/GBListDataObject_Item.h"
#include "Subsystem/Asset/GBAssetLoadSubsystem.h"

void UGBListEntryWidget_Item::OnOwingListDataObjectSet(UGBListDataObject_Base* InOwningListDataObject)
{
    if (CachedItemDataObject == nullptr)
    {
        CachedItemDataObject = Cast<UGBListDataObject_Item>(InOwningListDataObject);
        GB_NULL_CHECK(CachedItemDataObject);
    }

    if (CachedItemData == nullptr)
    {
        if (CachedItemDataObject->GetItemSlotData().ItemId.IsValid())
        {
            CachedItemDataObject->TryGetItemData(GetOwningPlayer(), FGBOnItemLoaded::CreateUObject(this, &UGBListEntryWidget_Item::OnItemDataLoaded));
        }
    }
    else
    {
        SetItemSlot(CachedItemDataObject->GetItemSlotData().Count, CachedItemData->Icon);
    }

    Super::OnOwingListDataObjectSet(InOwningListDataObject);
}

void UGBListEntryWidget_Item::OnOwningListDataObjectModified(UGBListDataObject_Base* InListDataObject, EGBOptionsListDataModifyReason ModifyReason)
{
    if (CachedItemDataObject == nullptr)
    {
        CachedItemDataObject = Cast<UGBListDataObject_Item>(InListDataObject);
        GB_NULL_CHECK(CachedItemDataObject);
    }

    if (CachedItemData == nullptr)
    {
        CachedItemDataObject->TryGetItemData(GetOwningPlayer(), FGBOnItemLoaded::CreateUObject(this, &UGBListEntryWidget_Item::OnItemDataLoaded));
    }
    else
    {
        SetItemSlot(CachedItemDataObject->GetItemSlotData().Count, CachedItemData->Icon);
    }
}

void UGBListEntryWidget_Item::SetItemSlot(int32 Count, const FSlateBrush& IconImage)
{
    CB_ItemSlot->SetButtonText(FText::FromString(FString::Printf(TEXT(" %d"), Count)));
    CB_ItemSlot->SetButtonDisplayImage(IconImage);
}

void UGBListEntryWidget_Item::OnItemDataLoaded(bool bSuccessd, UGBBaseItemData* ItemData)
{
    if (bSuccessd)
    {
        GB_NULL_CHECK(ItemData);

        CachedItemData = ItemData;
        SetItemSlot(CachedItemDataObject->GetItemSlotData().Count, CachedItemData->Icon);
    }
}
