// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BTDecorator_CanSprint.generated.h"

UENUM()
enum class EBTCustomObserverMode : uint8
{
    ValueChange UMETA(DisplayName = "On Value Change"),
    ResultChange UMETA(DisplayName = "On Result Change")
};

USTRUCT()
struct FCanSprintMemory
{
    GENERATED_BODY()
    FDelegateHandle Handle;
    bool            bLastResult = false;
};

UCLASS()
class GREEDBOUND_API UBTDecorator_CanSprint : public UBTDecorator
{
    GENERATED_BODY()
    
public:
    UBTDecorator_CanSprint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    virtual void                    InitializeFromAsset(UBehaviorTree& Asset) override;
    virtual void                    OnNodeActivation(FBehaviorTreeSearchData& SearchData) override;
    virtual void                    OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type Result) override;
    virtual bool                    CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

    virtual uint16                  GetInstanceMemorySize() const override { return sizeof(FCanSprintMemory); }

private:
    EBlackboardNotificationResult   OnBBValueChanged(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKey);

protected:
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector          StaminaKey;

    UPROPERTY(EditAnywhere, Category = "Flow Control")
    EBTCustomObserverMode           NotifyObserver = EBTCustomObserverMode::ValueChange;
};
