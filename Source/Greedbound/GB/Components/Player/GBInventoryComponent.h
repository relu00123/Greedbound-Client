// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/GBItemData.h"
#include "GBInventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnSlotsChangedDelegate, int32 Index, FPrimaryAssetId ItemAssetId, int32 Count);

/// PlayerState에서 가지는 컴포넌트
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GREEDBOUND_API UGBInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UGBInventoryComponent();

    const TArray<FGBInventorySlot>& GetItemLists() const { return InventoryList.Slots; }
    int32                           GetInventoryMaxCount() { return MaxSlotCount; }

    virtual void                    GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override final;
    void                            InitInventory(int32 InMaxSlotCount);

    UFUNCTION()
    void                            OnItemAdded(FPrimaryAssetId ItemDataId, int32 Count);

    // Server
    bool                            Server_MoveOrSwap(int32 FromIndex, int32 ToIndex);
    bool                            Server_Split(int32 FromIndex, int32 ToIndex, int32 SplitCount);
    bool                            Server_Merge(int32 FromIndex, int32 ToIndex);
    bool                            Server_RemoveCount(int32 Slot, int32 RemoveCount);

    UFUNCTION(Server, Reliable)
    void                            ServerReq_MoveOrSwap(int32 FromIndex, int32 ToIndex);

    UFUNCTION(Server, Reliable)
    void                            ServerReq_Split(int32 FromIndex, int32 ToIndex, int32 SplitCount);

    UFUNCTION(Server, Reliable)
    void                            ServerReq_Merge(int32 FromIndex, int32 ToIndex);

    UFUNCTION(Server, Reliable)
    void                            ServerReq_Use(int32 Slot);

protected:
    virtual void                    BeginPlay() override;

private:
    void                            MarkSlotDirty(int32 SlotIndex);
    void                            MarkArrayDirty();
    bool                            IsValidSlot(int32 Index) const;

public:
    // UI 갱신용 로컬 이벤트 (Only Client)
    FOnSlotsChangedDelegate         OnSlotsAdded;
    FOnSlotsChangedDelegate         OnSlotsChanged;
    FOnSlotsChangedDelegate         OnSlotsRemoved;

private:
    UPROPERTY(Replicated)
    FGBInventoryList                InventoryList;

    int32                           MaxSlotCount;
};
