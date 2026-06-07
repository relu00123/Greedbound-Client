// Fill out your copyright notice in the Description page of Project Settings.

#include "GBMonsterAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BTNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Define/GBAI.h"

void AGBMonsterAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    
    RunAI();
}

void AGBMonsterAIController::RunAI()
{
    UBlackboardComponent* BlackboardPtr = Blackboard.Get();
    if (UseBlackboard(BBAsset, BlackboardPtr))
    {
        Blackboard->SetValueAsVector(BBKEY_HOMEPOS, GetPawn()->GetActorLocation());

        bool RunResult = RunBehaviorTree(BTAsset);
        ensure(RunResult);
    }
}

void AGBMonsterAIController::StopAI()
{
    if (UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(GetBrainComponent()))
    {
        BTComponent->StopTree(EBTStopMode::Safe);
    }
}
