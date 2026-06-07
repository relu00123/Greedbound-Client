// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GBInventoryItemPopupWidget.generated.h"

class UTextBlock;
class UGBBaseItemData;

UCLASS()
class GREEDBOUND_API UGBInventoryItemPopupWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    void                    SetPopupByItemData(const UGBBaseItemData* ItemData, int32 ItemCount);

private:
    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock>  TB_ItemName;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock>  TB_ItemCount;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock>  TB_ItemDescription;
};
