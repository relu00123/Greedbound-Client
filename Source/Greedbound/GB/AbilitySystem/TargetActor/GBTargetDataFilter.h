// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities\GameplayAbilityTargetDataFilter.h"
#include "GameplayTagContainer.h"
#include "GBTargetDataFilter.generated.h"

USTRUCT(BlueprintType)
struct GREEDBOUND_API FGBGameplayTargetDataFilter : public FGameplayTargetDataFilter
{
    GENERATED_BODY()

public:
    virtual bool            FilterPassesForActor(const AActor* ActorToBeFiltered) const override final;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = Filter)
    FGameplayTagContainer   RequiredTags;
    
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = Filter)
    FGameplayTagContainer   FilterTags;
};
