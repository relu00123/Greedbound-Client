// Fill out your copyright notice in the Description page of Project Settings.

#include "GBTA_Trace.h"
#include "Define/GBCollision.h"
#include "GBTargetDataFilter.h"

void AGBTA_Trace::StartTargeting(UGameplayAbility* Ability)
{
    Super::StartTargeting(Ability);

    FHitResult HitResult;
    FGameplayTargetDataFilterHandle FilterHandle;
    FGBGameplayTargetDataFilter* NewFilter = new FGBGameplayTargetDataFilter();
    NewFilter->InitializeFilterContext(StartLocation.SourceActor);
    FilterHandle.Filter = TSharedPtr<FGBGameplayTargetDataFilter>(NewFilter);

    const FVector& Start = GetOwner()->GetActorLocation();
    const FVector End = Start + (GetOwner()->GetActorForwardVector() * MaxRange);
    FCollisionQueryParams Params;
    LineTraceWithFilter(HitResult, GetWorld(), FilterHandle, Start, End, GBCOLLISION_PROFILE_PLAYERAOE, Params);

    if(HitResult.bBlockingHit)
    {
        TargetDataReadyDelegate.Broadcast(MakeTargetData(HitResult));
    }
}
