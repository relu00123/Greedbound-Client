// Fill out your copyright notice in the Description page of Project Settings.

#include "GBItemData.h"
#include "Define/GBDefine.h"
#include "Components/Player/GBInventoryComponent.h"

bool FGBInventoryList::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
    return FFastArraySerializer::FastArrayDeltaSerialize<FGBInventorySlot, FGBInventoryList>(Slots, DeltaParams, *this);
}

void FGBInventoryList::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
    GB_NULL_CHECK(OwnerComponent.Get());

    for (int32 Added : AddedIndices)
    {
        OwnerComponent->OnSlotsAdded.Broadcast(Added, Slots.IsValidIndex(Added) ? Slots[Added].ItemId : FPrimaryAssetId(), FinalSize);
    }
}

void FGBInventoryList::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
    GB_NULL_CHECK(OwnerComponent.Get());

    for (int32 Changed : ChangedIndices)
    {
        OwnerComponent->OnSlotsChanged.Broadcast(Changed, Slots.IsValidIndex(Changed) ? Slots[Changed].ItemId : FPrimaryAssetId(), FinalSize);
    }
}

void FGBInventoryList::PostReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{
    GB_NULL_CHECK(OwnerComponent.Get());

    for (int32 Removed : RemovedIndices)
    {
        OwnerComponent->OnSlotsRemoved.Broadcast(Removed, Slots.IsValidIndex(Removed) ? Slots[Removed].ItemId : FPrimaryAssetId(), FinalSize);
    }
}

int32 FGBInventoryList::FindIndexByAssetId(const FPrimaryAssetId& Id)
{
    const int32 Size = Slots.Num();
    for (int Index = 0; Index < Size; ++Index)
    {
        if (Slots.IsValidIndex(Index) && Slots[Index].ItemId == Id)
        {
            return Index;
        }
    }

    return -1;
}

int32 FGBInventoryList::TryPush(FPrimaryAssetId Id, int32 Count)
{
    const int32 Index = FindIndexByAssetId(Id);
    if (Slots.IsValidIndex(Index))
    {
        // TODO 이후 아이템 소지량에 따라 칸 나누기
        Slots[Index].Count++;
    }
    else
    {
        int32 SlotOrder = -1;
        for (FGBInventorySlot& Slot : Slots)
        {
            SlotOrder++;
            if (Slot.IsEmpty())
            {
                Slot.ItemId = Id;
                Slot.Count = Count;
                return SlotOrder;
            }
        }
    }

    return Index;
}
