// Fill out your copyright notice in the Description page of Project Settings.

#include "GBGA_UnequippedAttack.h"
#include "Abilities/GameplayAbilityTargetActor_Trace.h"
#include "Define/GBTags.h"
#include "Components/GBCombatComponent.h"
#include "Define/GBDefine.h"

UGBGA_UnequippedAttack::UGBGA_UnequippedAttack(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
    : Super(ObjectInitializer)
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    EventReceiveTags.AddTag(GBTag::Event_Common_Attack);
}

void UGBGA_UnequippedAttack::OnEventReceived(const FGameplayTag EventTag, FGameplayEventData EventData)
{
    Super::OnEventReceived(EventTag, EventData);

    if (HasAuthority(&CurrentActivationInfo) == false)
    {
        return;
    }

    if (EventTag == GBTag::Event_Common_Attack)
    {
        AActor* AvatarActor = GetAvatarActorFromActorInfo();
        if (AvatarActor == nullptr)
        {
            return;
        }

        const FVector& Start = AvatarActor->GetActorLocation();
        const FVector& ForwardVector = AvatarActor->GetActorForwardVector();
        const FVector& End = Start + (ForwardVector * AttackRange);

        UWorld* World = GetWorld();
        GB_VALID_CHECK(World);

        GB_NULL_CHECK(CombatComponent);

        FHitResult HitResult;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(AvatarActor);

        const FName& AttackProfile = CombatComponent->GetAttackProfile();
        AGameplayAbilityTargetActor_Trace::LineTraceWithFilter(HitResult, World, FGameplayTargetDataFilterHandle(), Start, End, AttackProfile, Params);
        if (HitResult.bBlockingHit)
        {
            FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
            FGameplayAbilityTargetDataHandle TargetDataHandle;
            TargetDataHandle.Data.Add(TSharedPtr<FGameplayAbilityTargetData>(TargetData));

            OnTargetDataReady(TargetDataHandle);
        }
#if WITH_EDITOR
        DrawDebugLine(World, Start, End, HitResult.bBlockingHit ? FColor::Red : FColor::Green, false, 1.f, 0, 2.f);
#endif
    }
}
