#include "BTTask_MonsterAttack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/GBAbilitySystemHelper.h"
#include "Define/GBTags.h"
#include "Define/GBDefine.h"
#include "Interface/GAS/GBEndAbilityBindInterface.h"

UBTTask_MonsterAttack::UBTTask_MonsterAttack(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    NodeName = TEXT("Monster Attack");

    TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_MonsterAttack, TargetKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_MonsterAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    GB_NULL_CHECK_WITH_RETURN(AIController, EBTNodeResult::Failed);

    APawn* Pawn = AIController->GetPawn();
    GB_NULL_CHECK_WITH_RETURN(Pawn, EBTNodeResult::Failed);

    UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
    GB_NULL_CHECK_WITH_RETURN(BBComp, EBTNodeResult::Failed);

    AActor* Target = Cast<AActor>(BBComp->GetValueAsObject(TargetKey.SelectedKeyName));
    GB_NULL_CHECK_WITH_RETURN(Target, EBTNodeResult::Failed);

    IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Pawn);
    GB_NULL_CHECK_WITH_RETURN(ASCInterface, EBTNodeResult::Failed);

    UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
    GB_NULL_CHECK_WITH_RETURN(ASC, EBTNodeResult::Failed);

    TArray<FGameplayAbilitySpec*> Specs;
    FGBAbilitySystemHelper::GetAbilitySpecsByTag(ASC, GBTag::Trigger_Common_Primary, Specs);
    GB_CONDITION_CHECK_WITH_RETURN(Specs.Num() > 0, EBTNodeResult::Failed);

    for (FGameplayAbilitySpec* Spec : Specs)
    {
        for (UGameplayAbility* Instance : Spec->GetAbilityInstances())
        {
            GB_VALID_CHECK_WITH_RETURN(Instance, EBTNodeResult::Failed);

            GB_CONDITION_CHECK_WITH_RETURN(Instance->GetClass()->ImplementsInterface(UGBEndAbilityBindInterface::StaticClass()), EBTNodeResult::Failed);

            IGBEndAbilityBindInterface* BindInterface = Cast<IGBEndAbilityBindInterface>(Instance);
            GB_NULL_CHECK_WITH_RETURN(BindInterface, EBTNodeResult::Failed);

            FOnAbilityEndedDelegate* EndDelegate = BindInterface->GetOnAbilityEndedDelegate();
            GB_NULL_CHECK_WITH_RETURN(EndDelegate, EBTNodeResult::Failed);

            EndDelegate->AddLambda([this, &OwnerComp]()
                {
                    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
                });
        }
    }

    FGBAbilitySystemHelper::SendGameplayEventToSelf(GBTag::Trigger_Common_Primary, Pawn);

    return EBTNodeResult::InProgress;
}
