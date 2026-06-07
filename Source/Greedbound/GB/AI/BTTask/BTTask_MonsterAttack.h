// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MonsterAttack.generated.h"

UCLASS()
class GREEDBOUND_API UBTTask_MonsterAttack : public UBTTaskNode
{
    GENERATED_BODY()
    
public:
    UBTTask_MonsterAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector      TargetKey;
};
