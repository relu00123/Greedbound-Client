#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_Trace.generated.h"

UCLASS()
class GREEDBOUND_API UBTTask_Trace : public UBTTask_MoveTo
{
    GENERATED_BODY()
    
public:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
