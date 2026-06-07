// Fill out your copyright notice in the Description page of Project Settings.

#include "GBGA_EquippedMeleeAttack.h"
#include "Components/GBCombatComponent.h"
#include "AbilitySystem/AbilityTask/GBAT_WaitMeleeAttackTargetData.h"
#include "Define/GBDefine.h"
#include "Define/GBTags.h"

UGBGA_EquippedMeleeAttack::UGBGA_EquippedMeleeAttack(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
    : Super(ObjectInitializer)
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    EventReceiveTags.AddTag(GBTag::Event_Common_TrailAttackBegin);
    EventReceiveTags.AddTag(GBTag::Event_Common_TrailAttackEnd);
}

void UGBGA_EquippedMeleeAttack::OnEventReceived(const FGameplayTag EventTag, FGameplayEventData EventData)
{
    Super::OnEventReceived(EventTag, EventData);

    if (HasAuthority(&CurrentActivationInfo) == false)
    {
        return;
    }

    if (EventTag == GBTag::Event_Common_TrailAttackBegin)
    {
        if (IsValid(WaitMeleeAttackTargetDataTask))
        {
            WaitMeleeAttackTargetDataTask->Trace(true);
        }
        else
        {
            GB_NULL_CHECK(CombatComponent);

            UStaticMeshComponent* WeaponMesh = CombatComponent->GetWeaponMesh();
            GB_NULL_CHECK(WeaponMesh);

            WaitMeleeAttackTargetDataTask = UGBAT_WaitMeleeAttackTargetData::WaitForMeleeAttackTargetData(
                this, "WaitTargetDataTask", MeleeAttackTargetActorClass.Get(), WeaponMesh, WeaponMesh->GetComponentTransform());
            WaitMeleeAttackTargetDataTask->OnTargetDataReady.AddDynamic(this, &UGBGA_EquippedMeleeAttack::OnTargetDataReady);
            WaitMeleeAttackTargetDataTask->ReadyForActivation();
        }
    }
    else if (EventTag == GBTag::Event_Common_TrailAttackEnd)
    {
        if (IsValid(WaitMeleeAttackTargetDataTask))
        {
            WaitMeleeAttackTargetDataTask->Trace(false);
        }
    }
}
