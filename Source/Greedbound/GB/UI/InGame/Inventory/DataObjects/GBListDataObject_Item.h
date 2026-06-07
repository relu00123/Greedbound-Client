// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GB/UI/InGame/Options/DataObjects/GBListDataObject_Base.h"
#include "Data/GBItemData.h"
#include "Subsystem/Asset/GBAssetLoadSubsystem.h"
#include "GBListDataObject_Item.generated.h"



/**
 * 인벤토리 슬롯 당 가지고 있어야할 정보
 */
UCLASS()
class GREEDBOUND_API UGBListDataObject_Item : public UGBListDataObject_Base
{
	GENERATED_BODY()

public:
    const FGBInventorySlot&     GetItemSlotData() const { return ItemSlotData; }
    void                        SetItemSlotData(FPrimaryAssetId ItemId, int32 ItemCount);

    void                        TryGetItemData(const UObject* WorldContextObject, FGBOnItemLoaded OnLoadedDelegate);

private:
    UPROPERTY(Transient)
    TObjectPtr<UGBBaseItemData> CachedItemData;

    FGBInventorySlot            ItemSlotData;
};
