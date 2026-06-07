#include "BTDecorator_InAttackRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Interface/GBCombatInterface.h"
#include "Define/GBDefine.h"

UBTDecorator_InAttackRange::UBTDecorator_InAttackRange(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    NodeName = TEXT("Is In Attack Range");

    TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_InAttackRange, TargetKey), AActor::StaticClass());
}

bool UBTDecorator_InAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    GB_NULL_CHECK_WITH_RETURN(AIController, false);

    APawn* Pawn = AIController->GetPawn();
    GB_NULL_CHECK_WITH_RETURN(Pawn, false);

    UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
    AActor* Target = Cast<AActor>(BBComp->GetValueAsObject(TargetKey.SelectedKeyName));
    GB_NULL_CHECK_WITH_RETURN(Target, false);

    TScriptInterface<IGBCombatInterface> CombatInterface = Pawn;
    GB_NULL_CHECK_WITH_RETURN(CombatInterface, false);

    // CombatData 없애서 일단 하드코딩 박아놓음 추후 수정할 것
    const float AttackRange = 150.f;

    const float DistSqr = FVector::DistSquared(Pawn->GetActorLocation(), Target->GetActorLocation());

    return DistSqr <= FMath::Square(AttackRange);
}
