#include "BTService_UpdateStamina.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Attribute/GBSpeedAttributeSet.h"
#include "Interface/Monster/GBMonsterBaseInterface.h"
#include "Define/GBDefine.h"

UBTService_UpdateStamina::UBTService_UpdateStamina(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    Interval = 0.15f;
}

void UBTService_UpdateStamina::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    GB_NULL_CHECK(BB);

    AAIController* AI = OwnerComp.GetAIOwner();
    GB_NULL_CHECK(AI);

    APawn* Pawn = AI->GetPawn();
    GB_NULL_CHECK(Pawn);

    IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Pawn);
    GB_NULL_CHECK(ASCInterface);

    UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
    GB_NULL_CHECK(ASC);

    const UGBSpeedAttributeSet* SpeedSet = ASC->GetSet<UGBSpeedAttributeSet>();
    GB_NULL_CHECK(SpeedSet);

    BB->SetValueAsFloat(StaminaKey.SelectedKeyName, SpeedSet->GetCurrnetStamina());
}
