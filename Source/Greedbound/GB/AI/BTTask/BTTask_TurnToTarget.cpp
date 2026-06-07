#include "BTTask_TurnToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Define/GBDefine.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    NodeName = TEXT("Turn To Target");

    TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_TurnToTarget, TargetKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    GB_NULL_CHECK_WITH_RETURN(AIController, EBTNodeResult::Failed);

    APawn* ControlledPawn = AIController->GetPawn();
    GB_NULL_CHECK_WITH_RETURN(ControlledPawn, EBTNodeResult::Failed);

    UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
    AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(TargetKey.SelectedKeyName));
    GB_NULL_CHECK_WITH_RETURN(TargetActor, EBTNodeResult::Failed);

    FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(ControlledPawn->GetActorLocation(), TargetActor->GetActorLocation());
    FRotator NewControlRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);

    ControlledPawn->SetActorRotation(NewControlRotation);

    return EBTNodeResult::Succeeded;
}
