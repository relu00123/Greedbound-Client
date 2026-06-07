// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "GameplayTagContainer.h"
#include "GBCharacterSpawnPoint.generated.h"

UCLASS()
class GREEDBOUND_API AGBCharacterSpawnPoint : public APlayerStart
{
    GENERATED_BODY()
public:
    AGBCharacterSpawnPoint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // 팀/그룹 식별용 태그 ( 예 : Dungeon.Side.North 등)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
    FGameplayTag    GroupTag;

    // 이 포인트가 수용 가능한 최대 인원 수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn", meta = (ClampMin = "1", UIMin = "1"))
    int32           MaxSlots = 4;

    // 중심으로부터 슬롯 기본 반지름 (원형 배치)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn", meta = (ClampMin = "0.0"))
    float           SlotRadius = 180.f;

    // 각도 jitter (서로 겹치지 않도록) 
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn", meta = (ClampMin = "0.0", ClampMax = "45.0"))
    float           AngularJitterDeg = 8.f;

    // 반지름 jitter (중심/바깥쪽으로 살짝 펴짐)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn", meta = (ClampMin = "0.0", ClampMax = "0.25"))
    float           RadialJitterRatio = 0.1f;

    // 요청 인원수만큼 spqwn 트랜스폼 계산)
    UFUNCTION(BlueprintCallable, Category = "Spawn")
    void            MakeSpawnTransforms(int32 NumPlayers, TArray<FTransform>& Out) const;

protected:
    virtual void    OnConstruction(const FTransform& Transform) override;
};
