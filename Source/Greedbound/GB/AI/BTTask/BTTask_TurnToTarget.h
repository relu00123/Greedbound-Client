#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_TurnToTarget.generated.h"

UCLASS()
class GREEDBOUND_API UBTTask_TurnToTarget : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_TurnToTarget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector      TargetKey;
};
