// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_InAttackRange.generated.h"

UCLASS()
class GREEDBOUND_API UBTDecorator_InAttackRange : public UBTDecorator
{
    GENERATED_BODY()

public:
    UBTDecorator_InAttackRange(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    virtual bool            CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector  TargetKey;
};
