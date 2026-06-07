// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GBInventoryDataRegistry.generated.h"

class UGBListDataObject_Collection;
class UGBListDataObject_Base;

/**
 * 인벤토리 Tab 설정(최소 상단 Tab 때문이라도 필요함)
 */
UCLASS()
class GREEDBOUND_API UGBInventoryDataRegistry : public UObject
{
	GENERATED_BODY()

public:
    void                                                InitInventoryDataRegistry(int32 MaxInventoryCount);
    const TArray<UGBListDataObject_Collection*>&        GetRegisteredOptionsTabCollections() const;
    TArray<UGBListDataObject_Base*>                     GetListSourceItemBySelectedTabID(const FName& InSelectedTabID) const;

private:
    void                                                FindChildListDataRecursively(UGBListDataObject_Base* InParentData, TArray<UGBListDataObject_Base*>& OutChildDataList) const;
	
private:
    UPROPERTY(Transient)
    TArray<TObjectPtr<UGBListDataObject_Collection>>    RegisteredInventoryTabCollections;
};
