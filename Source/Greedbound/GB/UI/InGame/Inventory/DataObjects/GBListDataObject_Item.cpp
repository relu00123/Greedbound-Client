// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InGame/Inventory/DataObjects/GBListDataObject_Item.h"
#include "Subsystem/Asset/GBAssetLoadSubsystem.h"

void UGBListDataObject_Item::SetItemSlotData(FPrimaryAssetId ItemId, int32 ItemCount)
{
    if (ItemId.IsValid())
    {
        ItemSlotData.ItemId = ItemId;
        ItemSlotData.Count = ItemCount;

        NotifyListDataModified(this, EGBOptionsListDataModifyReason::DirectlyModified);
    }
}

void UGBListDataObject_Item::TryGetItemData(const UObject* WorldContextObject, FGBOnItemLoaded OnLoadedDelegate)
{
    if (CachedItemData == nullptr)
    {
        UGBAssetLoadSubsystem::Get(WorldContextObject)->LoadItemDataByAssetId(ItemSlotData.ItemId, FGBOnItemLoaded::CreateLambda([this, OnLoadedDelegate](bool bSuccess, UGBBaseItemData* LoadedItemData) {
                CachedItemData = LoadedItemData;
                OnLoadedDelegate.Execute(bSuccess, CachedItemData);
            }
        ));
    }
    else
    {
        OnLoadedDelegate.Execute(true, CachedItemData);
    }
}
