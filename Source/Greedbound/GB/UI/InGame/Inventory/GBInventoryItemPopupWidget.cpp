// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InGame/Inventory/GBInventoryItemPopupWidget.h"
#include "Components/TextBlock.h"
#include "Data/GBItemData.h"
#include "Define/GBDefine.h"

void UGBInventoryItemPopupWidget::SetPopupByItemData(const UGBBaseItemData* ItemData, int32 ItemCount)
{
    GB_NULL_CHECK(ItemData);

    if (TB_ItemName)
    {
        TB_ItemName->SetText(ItemData->Name);
    }

    if (TB_ItemCount)
    {
        TB_ItemCount->SetText(FText::FromString(FString::Printf(TEXT("%d"), ItemCount)));
    }

    if (TB_ItemDescription)
    {
        TB_ItemDescription->SetText(ItemData->Description);
    }
}
