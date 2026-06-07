// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GBMonsterAIController.generated.h"

class UAbilitySystemComponent;
class UBTCompositeNode;
class UBlackboardData;
class UBehaviorTree;
class UBTNode;

UCLASS()
class GREEDBOUND_API AGBMonsterAIController : public AAIController
{
    GENERATED_BODY()

protected:
    virtual void                OnPossess(APawn* InPawn) override;

private:
    void                        RunAI();
    void                        StopAI();

private:
    UPROPERTY(EditAnywhere)
    TObjectPtr<UBlackboardData> BBAsset;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UBehaviorTree>   BTAsset;
};
