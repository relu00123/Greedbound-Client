// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPatrolPos.h"
#include "Define/GBAI.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface/Monster/GBMonsterBaseInterface.h"
#include "Data/GBMonsterBaseData.h"

EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if (!IsValid(ControllingPawn))
    {
        return EBTNodeResult::Failed;
    }

    TScriptInterface<IGBMonsterBaseInterface> MonsterBaseInterface = ControllingPawn;
    if (!MonsterBaseInterface)
    {
        return EBTNodeResult::Failed;
    }

    const UGBMonsterBaseData* BaseData = MonsterBaseInterface->GetMonsterBaseData();
    const EGBPatrolType PatrolType = BaseData->PatrolData.PatrolType;

    switch (PatrolType)
    {
    case EGBPatrolType::Random:
        return HandleRandomPatrol(OwnerComp, BaseData);
    case EGBPatrolType::Loop:
        return HandleLoopPatrol(OwnerComp, BaseData);
    case EGBPatrolType::Fixed:
        return EBTNodeResult::Succeeded;
    default:
        return EBTNodeResult::Failed;
    }
}

EBTNodeResult::Type UBTTask_FindPatrolPos::HandleRandomPatrol(UBehaviorTreeComponent& OwnerComp, const UGBMonsterBaseData* BaseData)
{
    const FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BBKEY_HOMEPOS);
    const float Radius = BaseData->PatrolData.RandomPatrolRadius;

    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(OwnerComp.GetWorld());
    FNavLocation RandomPoint;
    if (NavSystem && NavSystem->GetRandomPointInNavigableRadius(Origin, Radius, RandomPoint))
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsVector(BBKEY_PATROLPOS, RandomPoint.Location);
        return EBTNodeResult::Succeeded;
    }
    return EBTNodeResult::Failed;
}

EBTNodeResult::Type UBTTask_FindPatrolPos::HandleLoopPatrol(UBehaviorTreeComponent& OwnerComp, const UGBMonsterBaseData* BaseData)
{
    const TArray<FVector>& Points = BaseData->PatrolData.PatrolPoints;
    if (Points.Num() == 0)
    {
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    const int32 CurrentIndex = BB->GetValueAsInt(BBKEY_PATROLINDEX);
    const int32 NextIndex = (CurrentIndex + 1) % Points.Num();

    BB->SetValueAsVector(BBKEY_PATROLPOS, Points[NextIndex]);
    BB->SetValueAsInt(BBKEY_PATROLINDEX, NextIndex);

    return EBTNodeResult::Succeeded;
}
