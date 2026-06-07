// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GBCharacterBase.h"
#include "Interface/Monster/GBMonsterBaseInterface.h"
#include "Define/GBCollision.h"
#include "GBMonsterCharacter.generated.h"

class UAbilitySystemComponent;
class UGBMonsterBaseData;
class UGBMonsterBehaviorComponent;
class UGBMovementStateComponent;
class UBTCompositeNode;
class UBTNode;
struct FGameplayEventData;

UCLASS()
class GREEDBOUND_API AGBMonsterCharacter : public AGBCharacterBase, public IGBMonsterBaseInterface
{
    GENERATED_BODY()

public:
    AGBMonsterCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    virtual void                            BeginPlay() override final;
    virtual void                            PossessedBy(AController* NewController) override final;
    virtual void                            Tick(float DeltaTime) override final;

    // IGBMonsterBaseInterface
    virtual UGBMonsterBaseData*             GetMonsterBaseData_Implementation() const override;

private:
    void                                    UpdateStopState();
    
private:
    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UGBMonsterBehaviorComponent> BehaviorComponent;
};
