// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Define/GBItemTypes.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "GBItemData.generated.h"

class UGameplayEffect;
class UGBInventoryComponent;
enum class EGBCharacterClassType : uint8;
enum class EGBEquipSlotType : uint8;

UCLASS()
class GREEDBOUND_API UGBBaseItemData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    virtual FPrimaryAssetId             GetPrimaryAssetId() const override { return FPrimaryAssetId(FName("ItemData"), *GetName()); }

public:
    UPROPERTY(EditDefaultsOnly)
    FText                               Name;

    UPROPERTY(EditDefaultsOnly)
    FText                               Description;

    UPROPERTY(EditDefaultsOnly)
    EGBItemType                         Type;

    UPROPERTY(EditDefaultsOnly)
    EGBItemGrade                        Grade;

    UPROPERTY(EditDefaultsOnly)
    uint8                               MaxStackSize;

    UPROPERTY(EditDefaultsOnly)
    float                               Weight;

    UPROPERTY(EditDefaultsOnly)
    FSlateBrush                         Icon;
};

UCLASS()
class GREEDBOUND_API UGBEquipmentItemData : public UGBBaseItemData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly)
    EGBEquipSlotType                        EquipSlot;

    UPROPERTY(EditDefaultsOnly, meta = (Categories = "Character.Player"))
    EGBCharacterClassType                   ClassType;

    UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UGameplayEffect>>    Effects;
};

UCLASS()
class GREEDBOUND_API UGBConsumableItemData : public UGBBaseItemData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly)
    float                                   UseDelay;

    UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UGameplayEffect>>    Effects;
};

// Item의 인스턴스 정보, 추후 요소 추가 대비해서 구조체로 둠
USTRUCT()
struct GREEDBOUND_API FGBItemInstanceData
{
    GENERATED_BODY()

public:
    UPROPERTY()
    uint8                               Durability;
};

USTRUCT(BlueprintType)
struct GREEDBOUND_API FGBInventorySlot : public FFastArraySerializerItem
{
    GENERATED_BODY()

public:
    bool                                IsValid() const { return ItemId.IsValid(); }
    bool                                IsEmpty() const { return ItemId.IsValid() || Count <= 0; }
    void                                MakeEmpty() { ItemId = FPrimaryAssetId(); Count = 0; }

public:
    UPROPERTY()
    FPrimaryAssetId                     ItemId;

    UPROPERTY()
    int32                               Count;

    UPROPERTY()
    FGBItemInstanceData                 InstanceData;
};

USTRUCT(BlueprintType)
struct GREEDBOUND_API FGBInventoryList : public FFastArraySerializer
{
    GENERATED_BODY()

public:
    // NetDeltaSerialize 내에서 FastArrayDeltaSerialize를 사용하는 FFastArraySerializer에서 
    // PostReplicatied~ 시그니처 함수가 있으면 Delta 적용 코드에서 호출함
    bool                                    NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParam);

    void                                    PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);
    void                                    PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize);
    void                                    PostReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize);

    int32                                   FindIndexByAssetId(const FPrimaryAssetId& Id);
    int32                                   TryPush(FPrimaryAssetId Id, int32 Count);

public:
    UPROPERTY()
    TArray<FGBInventorySlot>                Slots;

    UPROPERTY(NotReplicated)
    TWeakObjectPtr<UGBInventoryComponent>   OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FGBInventoryList> : public TStructOpsTypeTraitsBase2<FGBInventoryList>
{
    enum
    {
        WithNetDeltaSerializer = true,
    };
};
