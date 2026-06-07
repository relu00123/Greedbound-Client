// Fill out your copyright notice in the Description page of Project Settings.

#include "GBMonsterAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/GBMovementInterface.h"
#include "Define/GBDefine.h"
#include "Components/GBMovementStateComponent.h"

void UGBMonsterAnimInstance::NativeBeginPlay()
{
    Owner = Cast<ACharacter>(TryGetPawnOwner());
    GB_NULL_CHECK(Owner);

    Movement = Owner->GetCharacterMovement();

    TScriptInterface<IGBMovementInterface> MovementInterface = Owner;
    GB_NULL_CHECK(MovementInterface);

    MovementState = MovementInterface->GetMovementStateComponent();
}

void UGBMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (Movement == nullptr || MovementState == nullptr)
    {
        return;
    }

    Velocity = Movement->Velocity;
    GroundSpeed = Velocity.Size2D();
    bIsFalling = Movement->IsFalling();

    StanceState = MovementState->GetStanceState();
    StopState = GroundSpeed >= GBMovementConstants::MoveThreshold ? EGBStopState::Move : EGBStopState::Stop;
    MoveState = MovementState->GetMoveState();

    FVector NormalizedVelocity = Velocity.GetSafeNormal();
    Forward = NormalizedVelocity.Dot(Owner->GetActorForwardVector());
    Strafe = NormalizedVelocity.Dot(Owner->GetActorRightVector());
}
