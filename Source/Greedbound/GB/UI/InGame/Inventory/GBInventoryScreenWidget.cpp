// Fill out your copyright notice in the Description page of Project Settings.

#include "GBInventoryScreenWidget.h"
#include "CommonTileView.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Define/GBDefine.h"
#include "Data/GBItemData.h"
#include "Character/Player/GBPlayerState.h"
#include "Subsystem/UI/GBUISubsystem.h"
#include "Components/Player/GBInventoryComponent.h"
#include "UI/InGame/Component/GBTabListWidgetBase.h"
#include "UI/InGame/Inventory/GBInventoryDataRegistry.h"
#include "UI/InGame/Inventory/DataObjects/GBListDataObject_Item.h"
#include "UI/InGame/Options/DataObjects/GBListDataObject_Collection.h"
#include "GBInventoryItemPopupWidget.h"

UGBInventoryDataRegistry* UGBInventoryScreenWidget::GetOrCreateDataRegistry()
{
    if (InventoryDataRegistry == nullptr)
    {
        InventoryDataRegistry = NewObject<UGBInventoryDataRegistry>();
        InventoryDataRegistry->InitInventoryDataRegistry(CachedInventoryComponent->GetInventoryMaxCount());
    }

    GB_NULL_CHECK_WITH_RETURN(InventoryDataRegistry, nullptr);

    return InventoryDataRegistry;
}

void UGBInventoryScreenWidget::RefreshContainerItemList()
{
    GB_NULL_CHECK(CTV_Container);

    // TODO 컨테이너에서 interact로 ItemList 가져오기
}

void UGBInventoryScreenWidget::RefreshBagItemList()
{
    GB_NULL_CHECK(CTV_Bag);

    UGBInventoryComponent* InventoryComponent = CachedInventoryComponent.Get();
    GB_NULL_CHECK(InventoryComponent);

    const int32 InventoryMaxCount = InventoryComponent->GetInventoryMaxCount();
    const TArray<FGBInventorySlot>& SlotList = InventoryComponent->GetItemLists();
    const TArray<UGBListDataObject_Base*>& DataList = GetOrCreateDataRegistry()->GetListSourceItemBySelectedTabID(FName("InventoryTabCollection"));
    GB_CONDITION_CHECK(SlotList.Num() == InventoryMaxCount && DataList.Num() == InventoryMaxCount);

    for (int32 Index = 0; Index < InventoryMaxCount; ++Index)
    {
        UGBListDataObject_Item* ItemDataObject = Cast<UGBListDataObject_Item>(DataList[Index]);
        GB_NULL_CHECK(ItemDataObject);
        ItemDataObject->SetItemSlotData(SlotList[Index].ItemId, SlotList[Index].Count);
    }
}

void UGBInventoryScreenWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    // Inventory
    {
        GB_NULL_CHECK(CTL_InventoryTabListWidget);
        GB_NULL_CHECK(CTV_Bag);

        APlayerController* PlayerController = GetOwningPlayerController();
        GB_NULL_CHECK(PlayerController);

        AGBPlayerState* PlayerState = PlayerController->GetPlayerState<AGBPlayerState>();
        GB_NULL_CHECK(PlayerState);

        CachedInventoryComponent = PlayerState->GetInventoryComponent();
        GB_NULL_CHECK(CachedInventoryComponent);

        const TArray<UGBListDataObject_Base*>& DataList = GetOrCreateDataRegistry()->GetListSourceItemBySelectedTabID(FName("InventoryTabCollection"));
        CTV_Bag->SetListItems(DataList);

        CachedInventoryComponent->OnSlotsAdded.AddUObject(this, &UGBInventoryScreenWidget::OnItemSlotAdded);
        CachedInventoryComponent->OnSlotsChanged.AddUObject(this, &UGBInventoryScreenWidget::OnItemSlotChanged);
        CachedInventoryComponent->OnSlotsRemoved.AddUObject(this, &UGBInventoryScreenWidget::OnItemSlotRemoved);

        CTV_Bag->OnItemSelectionChanged().AddUObject(this, &UGBInventoryScreenWidget::OnTileViewItemSelected);
        CTV_Bag->OnItemIsHoveredChanged().AddUObject(this, &UGBInventoryScreenWidget::OnTileViewItemHovered);

        RefreshBagItemList();
    }

    // Map (TODO)
    {
        //CTL_InventoryTabListWidget->OnTabSelected.AddUnique(this, &UGBInventoryScreenWidget::OnInventoryTabSelected);
    }

    // Popup
    {
        if (ItemHoveredPopupWidgetClass)
        {
            ItemHoveredPopupWidget = CreateWidget<UGBInventoryItemPopupWidget>(GetOwningPlayer(), ItemHoveredPopupWidgetClass);
            if (ItemHoveredPopupWidget)
            {
                ItemHoveredPopupWidget->AddToViewport();
                ItemHoveredPopupWidget->SetVisibility(ESlateVisibility::Collapsed);
            }
        }
    }
}

UWidget* UGBInventoryScreenWidget::NativeGetDesiredFocusTarget() const
{
    if (CTV_Bag && CTV_Bag->GetNumItems() > 0)
    {
        return CTV_Bag;
    }

    if (CTL_InventoryTabListWidget)
    {
        return CTL_InventoryTabListWidget;
    }

    return Super::NativeGetDesiredFocusTarget();
}

void UGBInventoryScreenWidget::NativeOnActivated()
{
    Super::NativeOnActivated();

    UGBUISubsystem::Get(GetOwningPlayerController())->SetHUDVisible(false);

    const TArray<UGBListDataObject_Collection*>& RegisteredTabCollections = GetOrCreateDataRegistry()->GetRegisteredOptionsTabCollections();
    for (UGBListDataObject_Collection* TabCollection : RegisteredTabCollections)
    {
        if (TabCollection)
        {
            const FName& TabId = TabCollection->GetDataID();
            CTL_InventoryTabListWidget->RequestRegisterTab(TabId, TabCollection->GetDataDisplayName());
        }
    }
}

void UGBInventoryScreenWidget::NativeOnDeactivated()
{
    Super::NativeOnDeactivated();

    HideItemHoverPopup();
}

void UGBInventoryScreenWidget::OnItemSlotAdded(int32 SlotIndex, FPrimaryAssetId Id, int32 Count)
{
    // TODO 신규아이템 습득 시 강조효과
}

void UGBInventoryScreenWidget::OnItemSlotChanged(int32 SlotIndex, FPrimaryAssetId Id, int32 Count)
{
    if (Id.IsValid() == false)
    {
        return;
    }

    UGBListDataObject_Item* ItemDataObject = Cast<UGBListDataObject_Item>(CTV_Bag->GetItemAt(SlotIndex));
    GB_NULL_CHECK(ItemDataObject);

    ItemDataObject->SetItemSlotData(Id, Count);
}

void UGBInventoryScreenWidget::OnItemSlotRemoved(int32 SlotIndex, FPrimaryAssetId Id, int32 Count)
{
    // TODO 퀵슬롯에서 해제
}

void UGBInventoryScreenWidget::OnTileViewItemHovered(UObject* InHoveredItem, bool bWasHovered)
{
    if (bWasHovered == false)
    {
        HideItemHoverPopup();
        return;
    }

    GB_NULL_CHECK(InHoveredItem);
    GB_NULL_CHECK(ItemHoveredPopupWidget);

    UGBListDataObject_Item* ItemDataObject = Cast<UGBListDataObject_Item>(InHoveredItem);
    GB_NULL_CHECK(ItemDataObject);

    ItemDataObject->TryGetItemData(GetOwningPlayer(), FGBOnItemLoaded::CreateLambda(
        [this, ItemDataObject](bool bSuccess, UGBBaseItemData* LoadedItemData) 
        {
            if (bSuccess)
            {
                FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
                MousePosition += FVector2D(20.f, 0.f);
                ItemHoveredPopupWidget->SetPositionInViewport(MousePosition, false);
                ItemHoveredPopupWidget->SetPopupByItemData(LoadedItemData, ItemDataObject->GetItemSlotData().Count);
                ItemHoveredPopupWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
            }
        }
    ));
}

void UGBInventoryScreenWidget::OnTileViewItemSelected(UObject* InSelectedItem)
{
    //UGBListDataObject_Item* ItemDataobject = Cast<UGBListDataObject_Item>(InSelectedItem);
    //GB_NULL_CHECK(ItemDataobject);
}

void UGBInventoryScreenWidget::HideItemHoverPopup()
{
    if (ItemHoveredPopupWidget)
    {
        ItemHoveredPopupWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
}
