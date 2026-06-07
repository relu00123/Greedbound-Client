// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GBAT_WaitMeleeAttackTargetData.generated.h"

class UGameplayAbility;
class AGBTA_MeleeAttack;
struct FGameplayAbilityTargetDataHandle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGBWaitMeleeAttackTargetDataDelegate, const FGameplayAbilityTargetDataHandle&, TargetData);

UCLASS()
class GREEDBOUND_API UGBAT_WaitMeleeAttackTargetData : public UAbilityTask
{
    GENERATED_BODY()
    
public:
    UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
    static UGBAT_WaitMeleeAttackTargetData* WaitForMeleeAttackTargetData(
                                            UGameplayAbility* OwningAbility,
                                            FName TaskInstanceName,
                                            UClass* TargetActorClass,
                                            UStaticMeshComponent* WeaponMesh,
                                            FTransform SpawnTransform);

    UFUNCTION(BlueprintCallable)
    void                                    Trace(const bool bStart);

protected:
    virtual void                            Activate() override final;
    void                                    OnTargetDataRecieved(const FGameplayAbilityTargetDataHandle& Data) const;

    virtual void                            OnDestroy(bool bInOwnerFinished) override final;

public:
    UPROPERTY(BlueprintAssignable)
    FGBWaitMeleeAttackTargetDataDelegate    OnTargetDataReady;

private:
    UPROPERTY()
    TObjectPtr<UStaticMeshComponent>        WeaponMesh;
    
    UPROPERTY()
    TSubclassOf<AGameplayAbilityTargetActor> TargetActorClass;

    UPROPERTY()
    TObjectPtr<AGameplayAbilityTargetActor> TargetActor;

    FTransform                              TargetActorSpawnTransform;
    uint8                                   bTrace : 1;
};
