// Fill out your copyright notice in the Description page of Project Settings.

#include "GB/Components/Player/GBInventoryComponent.h"
#include "Define/GBDefine.h"
#include "Net/UnrealNetwork.h"
#include "Subsystem/Asset/GBAssetLoadSubsystem.h"

UGBInventoryComponent::UGBInventoryComponent()
{
    SetIsReplicatedByDefault(true);
}

void UGBInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGBInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(UGBInventoryComponent, InventoryList, COND_OwnerOnly);
}

void UGBInventoryComponent::InitInventory(int32 InMaxSlotCount)
{
    GB_CONDITION_CHECK(GetOwner() && GetOwner()->HasAuthority());

    MaxSlotCount = InMaxSlotCount;
    InventoryList.OwnerComponent = this;
    for (int32 Index = 0; Index < MaxSlotCount; ++Index)
    {
        FGBInventorySlot NewSlot = FGBInventorySlot();
        InventoryList.Slots.Add(NewSlot);
        InventoryList.Slots[Index].MakeEmpty();
        InventoryList.MarkItemDirty(InventoryList.Slots[Index]);
    }
}

bool UGBInventoryComponent::Server_MoveOrSwap(int32 FromIndex, int32 ToIndex)
{
    GB_CONDITION_CHECK_WITH_RETURN(GetOwner() && GetOwner()->HasAuthority(), false);
    GB_CONDITION_CHECK_WITH_RETURN(IsValidSlot(FromIndex) && IsValidSlot(ToIndex), false);

    FGBInventorySlot& FromSlot  = InventoryList.Slots[FromIndex];
    FGBInventorySlot& ToSlot    = InventoryList.Slots[ToIndex];

    if (FromSlot.IsEmpty())
    {
        return false;
    }

    Swap(FromSlot, ToSlot);

    MarkSlotDirty(FromIndex);
    MarkSlotDirty(ToIndex);

    return true;
}

bool UGBInventoryComponent::Server_Split(int32 FromIndex, int32 ToIndex, int32 SplitCount)
{
    GB_CONDITION_CHECK_WITH_RETURN(GetOwner() && GetOwner()->HasAuthority(), false);
    GB_CONDITION_CHECK_WITH_RETURN(IsValidSlot(FromIndex) && IsValidSlot(ToIndex), false);

    FGBInventorySlot& FromSlot = InventoryList.Slots[FromIndex];
    FGBInventorySlot& ToSlot = InventoryList.Slots[ToIndex];

    if (FromSlot.IsEmpty() || SplitCount <= 0 || SplitCount >= FromSlot.Count)
    {
        return false;
    }

    if (ToSlot.IsEmpty() == false)
    {
        return false;
    }

    ToSlot.ItemId = FromSlot.ItemId;
    ToSlot.Count = SplitCount;
    FromSlot.Count -= SplitCount;

    MarkSlotDirty(FromIndex);
    MarkSlotDirty(ToIndex);

    return false;
}

bool UGBInventoryComponent::Server_Merge(int32 FromIndex, int32 ToIndex)
{
    GB_CONDITION_CHECK_WITH_RETURN(GetOwner() && GetOwner()->HasAuthority(), false);
    GB_CONDITION_CHECK_WITH_RETURN(IsValidSlot(FromIndex) && IsValidSlot(ToIndex), false);

    FGBInventorySlot& FromSlot = InventoryList.Slots[FromIndex];
    FGBInventorySlot& ToSlot = InventoryList.Slots[ToIndex];

    if (FromSlot.IsEmpty() || ToSlot.IsEmpty())
    {
        return false;
    }

    if (FromSlot.ItemId != ToSlot.ItemId)
    {
        return false;
    }

    int32 MaxStack = -1;

    UGBAssetLoadSubsystem::Get(this)->LoadItemDataByAssetId(FromSlot.ItemId, 
        FGBOnItemLoaded::CreateWeakLambda(this, 
        [this, MaxStack](bool bSuccess, UGBBaseItemData* ItemData) mutable
        {
            if (bSuccess && ItemData)
            {
                MaxStack = ItemData->MaxStackSize;
            }
        })
    );

    const int32 CanAdd = MaxStack - ToSlot.Count;
    if (CanAdd <= 0)
    {
        return false;
    }

    const int32 MoveCount = FMath::Min(CanAdd, FromSlot.Count);
    ToSlot.Count += MoveCount;
    FromSlot.Count -= MoveCount;

    if (FromSlot.Count <= 0)
    {
        FromSlot.MakeEmpty();
    }

    MarkSlotDirty(FromIndex);
    MarkSlotDirty(ToIndex);

    return true;
}

bool UGBInventoryComponent::Server_RemoveCount(int32 SlotIndex, int32 RemoveCount)
{
    GB_CONDITION_CHECK_WITH_RETURN(GetOwner() && GetOwner()->HasAuthority() && IsValidSlot(SlotIndex), false);

    FGBInventorySlot& Slot = InventoryList.Slots[SlotIndex];
    if (Slot.IsEmpty() || RemoveCount <= 0)
    {
        return false;
    }

    if (RemoveCount >= Slot.Count)
    {
        Slot.MakeEmpty();
    }
    else
    {
        Slot.Count -= RemoveCount;
    }

    MarkSlotDirty(SlotIndex);

    return true;
}

void UGBInventoryComponent::ServerReq_MoveOrSwap_Implementation(int32 FromIndex, int32 ToIndex)
{
    ServerReq_MoveOrSwap(FromIndex, ToIndex);
}

void UGBInventoryComponent::ServerReq_Split_Implementation(int32 FromIndex, int32 ToIndex, int32 SplitCount)
{
    ServerReq_Split(FromIndex, ToIndex, SplitCount);
}

void UGBInventoryComponent::ServerReq_Merge_Implementation(int32 FromIndex, int32 ToIndex)
{
    Server_Merge(FromIndex, ToIndex);
}

void UGBInventoryComponent::ServerReq_Use_Implementation(int32 SlotIndex)
{
    ServerReq_Use(SlotIndex);
}

void UGBInventoryComponent::MarkSlotDirty(int32 SlotIndex)
{
    GB_CONDITION_CHECK(InventoryList.Slots.IsValidIndex(SlotIndex));

    InventoryList.MarkItemDirty(InventoryList.Slots[SlotIndex]);
}

void UGBInventoryComponent::MarkArrayDirty()
{
    InventoryList.MarkArrayDirty();
}

bool UGBInventoryComponent::IsValidSlot(int32 Index) const
{
    return InventoryList.Slots.IsValidIndex(Index);
}

void UGBInventoryComponent::OnItemAdded(FPrimaryAssetId ItemDataId, int32 Count)
{
    GB_CONDITION_CHECK(ItemDataId.IsValid());

    if (GetOwner()->HasAuthority())
    {
        const int32 SlotIndex = InventoryList.TryPush(ItemDataId, Count);
        if (InventoryList.Slots.IsValidIndex(SlotIndex) == false)
        {
            return;
        }

        InventoryList.MarkItemDirty(InventoryList.Slots[SlotIndex]);
    }
}
