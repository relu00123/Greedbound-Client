#include "BTDecorator_CanSprint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Interface/Monster/GBMonsterBaseInterface.h"
#include "Data/GBMonsterBaseData.h"
#include "Define/GBDefine.h"

UBTDecorator_CanSprint::UBTDecorator_CanSprint(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    NodeName = TEXT("CanSprint");

    bNotifyActivation = true;
    bNotifyDeactivation = true;
}

void UBTDecorator_CanSprint::InitializeFromAsset(UBehaviorTree& Asset)
{
    Super::InitializeFromAsset(Asset);

    if (UBlackboardData* BBAsset = GetBlackboardAsset())
    {
        StaminaKey.ResolveSelectedKey(*BBAsset);
    }
}

void UBTDecorator_CanSprint::OnNodeActivation(FBehaviorTreeSearchData& SearchData)
{
    Super::OnNodeActivation(SearchData);

    uint8* RawMem = SearchData.OwnerComp.GetNodeMemory(this, SearchData.OwnerComp.FindInstanceContainingNode(this));
    FCanSprintMemory* Mem = CastInstanceNodeMemory<FCanSprintMemory>(RawMem);

    UBlackboardComponent* BB = SearchData.OwnerComp.GetBlackboardComponent();
    GB_NULL_CHECK(BB);

    // KeyID는 ResolveSelectedKey 이후 GetSelectedKeyID() 사용. Key Name 을 사용하는 경우에는 ResolveSelectedKey 안해도 됨.
    Mem->Handle = BB->RegisterObserver(StaminaKey.GetSelectedKeyID(), this, FOnBlackboardChangeNotification::CreateUObject(this, &UBTDecorator_CanSprint::OnBBValueChanged));

    Mem->bLastResult = CalculateRawConditionValue(SearchData.OwnerComp, nullptr);
}

void UBTDecorator_CanSprint::OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type Result)
{
    Super::OnNodeDeactivation(SearchData, Result);

    uint8* RawMem = SearchData.OwnerComp.GetNodeMemory(this, SearchData.OwnerComp.FindInstanceContainingNode(this));
    FCanSprintMemory* Mem = CastInstanceNodeMemory<FCanSprintMemory>(RawMem);

    if (UBlackboardComponent* BB = SearchData.OwnerComp.GetBlackboardComponent())
    {
        if (Mem->Handle.IsValid())
        {
            BB->UnregisterObserver(StaminaKey.GetSelectedKeyID(), Mem->Handle);
            Mem->Handle.Reset();
        }
    }
}

EBlackboardNotificationResult UBTDecorator_CanSprint::OnBBValueChanged(const UBlackboardComponent& Blackboard, FBlackboard::FKey /*ChangedKey*/)
{
    if (UBehaviorTreeComponent* BTC = Cast<UBehaviorTreeComponent>(Blackboard.GetBrainComponent()))
    {
        uint8* RawMem = BTC->GetNodeMemory(this, BTC->FindInstanceContainingNode(this));
        FCanSprintMemory* Mem = CastInstanceNodeMemory<FCanSprintMemory>(RawMem);

        if (NotifyObserver == EBTCustomObserverMode::ValueChange)
        {
            BTC->RequestExecution(this);
        }
        else
        {
            const bool NewResult = CalculateRawConditionValue(*BTC, (uint8*)Mem);
            if (NewResult != Mem->bLastResult)
            {
                Mem->bLastResult = NewResult;
                BTC->RequestExecution(this);
            }
        }
    }
    return EBlackboardNotificationResult::ContinueObserving; // 계속 관찰
}

bool UBTDecorator_CanSprint::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    const UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    GB_VALID_CHECK_WITH_RETURN(BB, false);

    AAIController* AIController = OwnerComp.GetAIOwner();
    GB_VALID_CHECK_WITH_RETURN(AIController, false);

    APawn* Pawn = AIController->GetPawn();
    GB_VALID_CHECK_WITH_RETURN(Pawn, false);

    TScriptInterface<IGBMonsterBaseInterface> MonsterBaseInterface = Pawn;
    GB_NULL_CHECK_WITH_RETURN(MonsterBaseInterface, false);

    const UGBMonsterBaseData* BaseData = MonsterBaseInterface->GetMonsterBaseData();
    GB_NULL_CHECK_WITH_RETURN(BaseData, false);

    const float SprintThreshold = BaseData->PerceptionData.SprintThreshold;
    const float CurrentStamina = BB->GetValueAsFloat(StaminaKey.SelectedKeyName);

    return CurrentStamina >= SprintThreshold;
}
