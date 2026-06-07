#include "BTTask_Trace.h"
#include "AIController.h"
#include "Define/GBTags.h"
#include "Define/GBDefine.h"
#include "AbilitySystem/GBAbilitySystemHelper.h"

EBTNodeResult::Type UBTTask_Trace::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    GB_NULL_CHECK_WITH_RETURN(AIController, EBTNodeResult::Failed);

    APawn* AIPawn = AIController->GetPawn();
    GB_NULL_CHECK_WITH_RETURN(AIPawn, EBTNodeResult::Failed);

    FGBAbilitySystemHelper::SendGameplayEventToSelf(GBTag::Trigger_Common_Sprint, AIPawn);

    return Super::ExecuteTask(OwnerComp, NodeMemory);
}
