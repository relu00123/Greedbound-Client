// Fill out your copyright notice in the Description page of Project Settings.

#include "GBAT_JumpAndWaitForLanding.h"
#include "GameFramework/Character.h"

UGBAT_JumpAndWaitForLanding* UGBAT_JumpAndWaitForLanding::CreateTask(UGameplayAbility* OwningAbility)
{
    UGBAT_JumpAndWaitForLanding* NewTask = NewAbilityTask<UGBAT_JumpAndWaitForLanding>(OwningAbility);
    return NewTask;
}

void UGBAT_JumpAndWaitForLanding::Activate()
{
    Super::Activate();

    ACharacter* Character = CastChecked<ACharacter>(GetAvatarActor());
    Character->LandedDelegate.AddDynamic(this, &UGBAT_JumpAndWaitForLanding::OnLandedCallback);
    Character->Jump();

    SetWaitingOnAvatar();
}

void UGBAT_JumpAndWaitForLanding::OnDestroy(bool AbilityEnded)
{
    ACharacter* Character = CastChecked<ACharacter>(GetAvatarActor());
    Character->LandedDelegate.RemoveDynamic(this, &UGBAT_JumpAndWaitForLanding::OnLandedCallback);

    Super::OnDestroy(AbilityEnded);
}

void UGBAT_JumpAndWaitForLanding::OnLandedCallback(const FHitResult& Hit)
{
    if (ShouldBroadcastAbilityTaskDelegates())
    {
        OnComplete.Broadcast();
    }
}
