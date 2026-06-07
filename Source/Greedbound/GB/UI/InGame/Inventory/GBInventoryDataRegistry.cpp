// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InGame/Inventory/GBInventoryDataRegistry.h"
#include "Define/GBDefine.h"
#include "UI/InGame/Options/DataObjects/GBListDataObject_Collection.h"
#include "UI/InGame/Inventory/DataObjects/GBListDataObject_Item.h"

void UGBInventoryDataRegistry::InitInventoryDataRegistry(int32 MaxInventoryCount)
{
    UGBListDataObject_Collection* InventoryTabCollection = NewObject<UGBListDataObject_Collection>();
    InventoryTabCollection->SetDataID(FName("InventoryTabCollection"));
    InventoryTabCollection->SetDataDisplayName(FText::FromString(TEXT("Inventory")));

    for (int32 Index = 0; Index < MaxInventoryCount; ++Index)
    {
        UGBListDataObject_Item* ItemData = NewObject<UGBListDataObject_Item>();
        ItemData->SetDataID(FName("ItemData"));
        ItemData->SetDataDisplayName(FText::FromString(TEXT("Item")));

        InventoryTabCollection->AddChildListData(ItemData);
    }

    RegisteredInventoryTabCollections.Add(InventoryTabCollection);

    UGBListDataObject_Collection* TestTabCollection = NewObject<UGBListDataObject_Collection>();
    TestTabCollection->SetDataID(FName("TestTabCollection"));
    TestTabCollection->SetDataDisplayName(FText::FromString(TEXT("Test")));

    RegisteredInventoryTabCollections.Add(TestTabCollection);
}

const TArray<UGBListDataObject_Collection*>& UGBInventoryDataRegistry::GetRegisteredOptionsTabCollections() const
{
    return RegisteredInventoryTabCollections;
}

TArray<UGBListDataObject_Base*> UGBInventoryDataRegistry::GetListSourceItemBySelectedTabID(const FName& InSelectedTabID) const
{
    const TObjectPtr<UGBListDataObject_Collection>* FoundTabCollectionPtr = RegisteredInventoryTabCollections.FindByPredicate(
        [InSelectedTabID](UGBListDataObject_Collection* AvaliableTabCollection)->bool
        {
            return AvaliableTabCollection->GetDataID() == InSelectedTabID;
        }
    );

    GB_NULL_CHECK_WITH_RETURN(FoundTabCollectionPtr, TArray<UGBListDataObject_Base*>());

    UGBListDataObject_Collection* FoundTabCollection = *FoundTabCollectionPtr;

    const TArray<UGBListDataObject_Base*>& ChildListDataList = FoundTabCollection->GetAllChildListData();
    TArray<UGBListDataObject_Base*> AllChildListItems;

    for (UGBListDataObject_Base* ChildListData : ChildListDataList)
    {
        if (ChildListData)
        {
            AllChildListItems.Add(ChildListData);

            if (ChildListData->HasAnyChildListData())
            {
                FindChildListDataRecursively(ChildListData, AllChildListItems);
            }
        }
    }

    return AllChildListItems;
}

void UGBInventoryDataRegistry::FindChildListDataRecursively(UGBListDataObject_Base* InParentData, TArray<UGBListDataObject_Base*>& OutChildDataList) const
{
    GB_CONDITION_CHECK(InParentData != nullptr && InParentData->HasAnyChildListData());

    const TArray<UGBListDataObject_Base*>& ChildListDataList = InParentData->GetAllChildListData();
    for (UGBListDataObject_Base* SubShildListData : ChildListDataList)
    {
        if (SubShildListData)
        {
            OutChildDataList.Add(SubShildListData);

            if (SubShildListData->HasAnyChildListData())
            {
                FindChildListDataRecursively(SubShildListData, OutChildDataList);
            }
        }
    }
}
