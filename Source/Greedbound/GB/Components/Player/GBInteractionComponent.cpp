// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/Player/GBInteractionComponent.h"
#include "KismetTraceUtils.h"
#include "Define/GBCollision.h"
#include "Interface/GBInteractable.h"
#include "Define/GBItemTypes.h"
#include "Define/GBDefine.h"

UGBInteractionComponent::UGBInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGBInteractionComponent::Interact()
{
    if (CurrentFocused == nullptr)
    {
        return;
    }

    const EGBInteractionType InteractionType = IGBInteractable::Execute_GetInteractionType(CurrentFocused.GetObject());
    if (InteractionType == EGBInteractionType::None)
    {
        GB_LOG(TEXT("InteractionType is Invalid!"));
    }
    else if (InteractionType == EGBInteractionType::Item)
    {
        TOptional<FPrimaryAssetId> ItemId = IGBInteractable::Execute_Interact(CurrentFocused.GetObject(), GetOwner());
        if (ItemId.IsSet())
        {
            OnInteractedWithItem.Broadcast(ItemId.GetValue(), 1);
        }
    }
    else if (InteractionType == EGBInteractionType::Activate)
    {

    }

    IGBInteractable::Execute_LookAway(CurrentFocused.GetObject());
    CurrentFocused = nullptr;
}

void UGBInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // TODO InputMode가 Game일 때만 실행하게 하기
    AActor* InteractableActor = FindNearestInteractable();
    if (CurrentFocused != InteractableActor)
    {
        if (CurrentFocused)
        {
            IGBInteractable::Execute_LookAway(CurrentFocused.GetObject());
        }

        CurrentFocused = InteractableActor;

        if (CurrentFocused)
        {
            IGBInteractable::Execute_LookAt(CurrentFocused.GetObject());
        }
    }
}

AActor* UGBInteractionComponent::FindNearestInteractable()
{
    FVector ViewLocation;
    FRotator ViewRotation;
    GetOwner()->GetActorEyesViewPoint(ViewLocation, ViewRotation);
    FVector ViewForward = ViewRotation.Vector();

    TArray<FHitResult> OutHits;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    const FVector Start = ViewLocation;
    const FVector End = ViewLocation + ViewForward * InteractionDistance;

    GetWorld()->SweepMultiByChannel(
        OutHits, Start, End,
        FQuat::Identity, GBCOLLISIONCHANNEL_PLAYERINTERACT, FCollisionShape::MakeSphere(InteractionRadius), Params
    );

    if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
    {
        DrawDebugSweptSphere(GetWorld(), Start, End, InteractionRadius, OutHits.IsEmpty() ? FColor::Green : FColor::Red, false, 0.01f);
    }

    AActor* NearestActor = nullptr;
    float BestScore = -1.f;

    for (const FHitResult& Hit : OutHits)
    {
        if (Hit.GetActor() && Hit.GetActor()->Implements<UGBInteractable>())
        {
            const FVector ToTarget = (Hit.GetActor()->GetActorLocation() - ViewLocation).GetSafeNormal();
            const float Dot = FVector::DotProduct(ViewForward, ToTarget);

            if (Dot < InteractionThreshold && Dot > BestScore)
            {
                BestScore = Dot;
                NearestActor = Hit.GetActor();
            }
        }
     }

    return NearestActor;
}

