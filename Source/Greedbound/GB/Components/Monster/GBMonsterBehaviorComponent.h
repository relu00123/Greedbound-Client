// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GBMonsterBehaviorComponent.generated.h"

class UGBMonsterBaseData;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GREEDBOUND_API UGBMonsterBehaviorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UGBMonsterBaseData*             GetMonsterBaseData() const;

private:
    UPROPERTY(EditDefaultsOnly, Category = "AttackMontage")
    TObjectPtr<UGBMonsterBaseData>  MonsterBaseData;
};
