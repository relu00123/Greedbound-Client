// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Detect.generated.h"

UCLASS()
class GREEDBOUND_API UBTService_Detect : public UBTService
{
    GENERATED_BODY()
    
public:
    UBTService_Detect(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    virtual void            TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector  TargetKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector  LastSensedTimeKey;
};
