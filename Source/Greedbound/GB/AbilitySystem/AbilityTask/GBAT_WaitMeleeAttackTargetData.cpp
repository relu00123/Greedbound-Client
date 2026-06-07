// Fill out your copyright notice in the Description page of Project Settings.

#include "GBAT_WaitMeleeAttackTargetData.h"
#include "Define/GBDefine.h"

UGBAT_WaitMeleeAttackTargetData* UGBAT_WaitMeleeAttackTargetData::WaitForMeleeAttackTargetData(UGameplayAbility* OwningAbility, FName TaskInstanceName, UClass* TargetActorClass, UStaticMeshComponent* WeaponMesh, FTransform SpawnTransform)
{
    UGBAT_WaitMeleeAttackTargetData* MyObj = NewAbilityTask<UGBAT_WaitMeleeAttackTargetData>(OwningAbility, TaskInstanceName);
    MyObj->WeaponMesh = WeaponMesh;
    MyObj->TargetActorClass = TargetActorClass;
    MyObj->TargetActorSpawnTransform = SpawnTransform;

    return MyObj;
}

void UGBAT_WaitMeleeAttackTargetData::Activate()
{
    Super::Activate();

    GB_NULL_CHECK(Ability);
    GB_NULL_CHECK(TargetActorClass);

    TargetActor = GetWorld()->SpawnActorDeferred<AGameplayAbilityTargetActor>(TargetActorClass, TargetActorSpawnTransform);
    GB_NULL_CHECK(TargetActor);

    TargetActor->SourceActor = Ability->GetOwningActorFromActorInfo();
    TargetActor->StartLocation.SourceComponent = WeaponMesh;
    TargetActor->StartLocation.SourceAbility = Ability;
    TargetActor->StartLocation.SourceActor = Ability->GetAvatarActorFromActorInfo();

    TargetActor->FinishSpawning(TargetActorSpawnTransform);
    TargetActor->bDebug = true;

    TargetActor->TargetDataReadyDelegate.AddUObject(this, &UGBAT_WaitMeleeAttackTargetData::OnTargetDataRecieved);
    TargetActor->StartTargeting(Ability);
}

void UGBAT_WaitMeleeAttackTargetData::OnTargetDataRecieved(const FGameplayAbilityTargetDataHandle& Data) const
{
    OnTargetDataReady.Broadcast(Data);
}

void UGBAT_WaitMeleeAttackTargetData::OnDestroy(bool bInOwnerFinished)
{
    Super::OnDestroy(bOwnerFinished);

    if (TargetActor)
    {
        TargetActor->AGameplayAbilityTargetActor::CancelTargeting();
        TargetActor = nullptr;
    }
}

void UGBAT_WaitMeleeAttackTargetData::Trace(const bool bStart)
{
    GB_NULL_CHECK(TargetActor);
    if (bStart)
    {
        TargetActor->StartTargeting(Ability);
    }
    else
    {
        TargetActor->CancelTargeting();
    }
}
