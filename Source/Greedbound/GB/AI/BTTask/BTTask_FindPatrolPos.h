// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindPatrolPos.generated.h"

class UGBMonsterBaseData;

UCLASS()
class GREEDBOUND_API UBTTask_FindPatrolPos : public UBTTaskNode
{
    GENERATED_BODY()
    
public:
    virtual EBTNodeResult::Type    ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    EBTNodeResult::Type            HandleRandomPatrol(UBehaviorTreeComponent& OwnerComp, const UGBMonsterBaseData* BaseData);
    EBTNodeResult::Type            HandleLoopPatrol(UBehaviorTreeComponent& OwnerComp, const UGBMonsterBaseData* BaseData);
};
