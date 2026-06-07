// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GBMonsterBaseData.generated.h"

UENUM()
enum class EGBPatrolType : uint8
{
    Random,
    Loop,
    Fixed,    // 제자리 고정
    Count
};

USTRUCT(BlueprintType)
struct FMonsterPatrolData
{
    GENERATED_BODY()

public:
    FMonsterPatrolData() = default;

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FVector> PatrolPoints;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float           RandomPatrolRadius = 500.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EGBPatrolType   PatrolType         = EGBPatrolType::Random;
};

USTRUCT(BlueprintType)
struct FMonsterPerceptionData
{
    GENERATED_BODY()
    
public:
    FMonsterPerceptionData() = default;
    
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float SightRadius            = 1000.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float LoseSightRadius        = 1200.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float FieldOfView            = 200.f;

    // 타겟을 시야에서 놓친 후에 기억하고 행동을 유지하는 시간
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float TargetRetentionTime    = 3.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float SprintThreshold        = 0.4f;
};

UCLASS()
class GREEDBOUND_API UGBMonsterBaseData : public UDataAsset
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
    FMonsterPatrolData      PatrolData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
    FMonsterPerceptionData  PerceptionData;
};
