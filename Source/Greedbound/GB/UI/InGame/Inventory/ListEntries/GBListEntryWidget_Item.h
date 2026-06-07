// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/InGame/Options/ListEntries/GBListEntryWidget_Base.h"
#include "GBListEntryWidget_Item.generated.h"

class UGBCommonButtonBase;
class UImage;
class UGBBaseItemData;
class UGBListDataObject_Item;
struct FGBInventorySlot;

/**
 * InventorySlot
 */

UCLASS()
class GREEDBOUND_API UGBListEntryWidget_Item : public UGBListEntryWidget_Base
{
	GENERATED_BODY()

public:
    void                                SetSlotIndex(int32 Index) { SlotIndex = Index; }
    int32                               GetSlotIndex() const { return SlotIndex; }

private:
    void                                OnItemDataLoaded(bool bSuccessd, UGBBaseItemData* ItemData);
    void                                OnOwingListDataObjectSet(UGBListDataObject_Base* InOwningListDataObject);
    void                                OnOwningListDataObjectModified(UGBListDataObject_Base* InListDataObject, EGBOptionsListDataModifyReason ModifyReason);

    void                                SetItemSlot(int32 Count, const FSlateBrush& IconImage);

private:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
    TObjectPtr<UGBCommonButtonBase>     CB_ItemSlot;

    int32                               SlotIndex;

    UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UGBListDataObject_Item>  CachedItemDataObject;

    UPROPERTY(Transient)
    TObjectPtr<UGBBaseItemData>         CachedItemData;
};
