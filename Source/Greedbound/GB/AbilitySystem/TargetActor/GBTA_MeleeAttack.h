// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GBTA_MeleeAttack.generated.h"

UCLASS()
class GREEDBOUND_API AGBTA_MeleeAttack : public AGameplayAbilityTargetActor
{
    GENERATED_BODY()

public:
    AGBTA_MeleeAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void                StartTargeting(UGameplayAbility* Ability) override final;
    virtual void                CancelTargeting() override final;
    
protected:
    virtual void                Tick(float DeltaSeconds) override final;

private:
    UPROPERTY(EditDefaultsOnly)
    TArray<FName>               TraceSocketNames;

    UPROPERTY(EditDefaultsOnly)
    FVector                     TraceSize = FVector(5.f);

    TMap<FName, FVector>        TraceSocketPrevLocations;
    TSet<TObjectPtr<AActor>>    AlreadyHitActors;
};
