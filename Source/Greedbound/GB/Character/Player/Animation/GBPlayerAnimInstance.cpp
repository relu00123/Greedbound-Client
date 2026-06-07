
#include "GBPlayerAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Define/GBDefine.h"
#include "Define/GBAnimSectionName.h"
#include "Interface/GBMovementInterface.h"
#include "Interface/GBCombatInterface.h"
#include "Components/GBMovementStateComponent.h"
#include "Components/GBCombatComponent.h"

void UGBPlayerAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();

    Owner = Cast<ACharacter>(TryGetPawnOwner());
    GB_NULL_CHECK(Owner);

    Movement = Owner->GetCharacterMovement();

    MovementState = IGBMovementInterface::Execute_GetMovementStateComponent(Owner);
    GB_NULL_CHECK(MovementState);

    Combat = IGBCombatInterface::Execute_GetCombatComponent(Owner);
    GB_NULL_CHECK(Combat);
}

void UGBPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (Movement == nullptr || MovementState == nullptr)
    {
        return;
    }

    Velocity    = Movement->Velocity;
    GroundSpeed = Velocity.Size2D();
    bIsFalling  = Movement->IsFalling();
    bIsJumping  = bIsFalling && (Velocity.Z > JumpingThreshould);

    const FVector NormalizedVelocity = Velocity.GetSafeNormal();
    Forward = NormalizedVelocity.Dot(Owner->GetActorForwardVector());
    Strafe  = NormalizedVelocity.Dot(Owner->GetActorRightVector());

    LastRotation    = CurrentRotation;
    CurrentRotation = Owner->GetActorRotation();
    DeltaRotation   = UKismetMathLibrary::NormalizedDeltaRotator(CurrentRotation, LastRotation);

    StanceState = MovementState->GetStanceState();
    StopState   = GroundSpeed >= GBMovementConstants::MoveThreshold ? EGBStopState::Move : EGBStopState::Stop;
    MoveState   = MovementState->GetMoveState();

    UpdateTurnInPlace();
}

void UGBPlayerAnimInstance::UpdateTurnInPlace()
{
    if (Owner == nullptr || Movement == nullptr || MovementState == nullptr)
    {
        return;
    }

    if (StopState != EGBStopState::Stop)
    {
        Movement->bUseControllerDesiredRotation = true;
        return;
    }

    if (Combat->GetCombatState() != EGBCombatState::Idle)
    {
        Movement->bUseControllerDesiredRotation = false;
        return;
    }

    UAnimMontage* TIPMontage = MovementState->GetTurnInPlaceMontage();
    GB_NULL_CHECK(TIPMontage);

    const float DeltaYaw = UKismetMathLibrary::NormalizedDeltaRotator(Owner->GetControlRotation(), Owner->GetActorRotation()).Yaw;

    if (Owner->GetLocalRole() >= ROLE_AutonomousProxy)
    {
        if (FMath::Abs(DeltaYaw) <= GBMovementConstants::TurnThresholdForAutonomous)
        {
            if (Montage_IsPlaying(TIPMontage) == false)
            {
                Movement->bUseControllerDesiredRotation = false;
            }
            return;
        }

        Movement->bUseControllerDesiredRotation = true;

        if (Montage_IsPlaying(TIPMontage))
        {
            return;
        }

        const FName SectionName = DeltaYaw > 0.f ? GBANIMSECTIONNAME_TURN_RIGHT : GBANIMSECTIONNAME_TURN_LEFT;
        Owner->PlayAnimMontage(TIPMontage, 1.f, SectionName);
    }
    else
    {
        LastRotation = Owner->GetActorRotation();
        const float YawDelta = DeltaRotation.Yaw;

        if (Montage_IsPlaying(TIPMontage))
        {
            return;
        }

        if (FMath::Abs(YawDelta) < GBMovementConstants::TurnThresholdForSimulatedProxy)
        {
            return;
        }

        const FName SectionName = YawDelta > 0.f ? GBANIMSECTIONNAME_TURN_RIGHT : GBANIMSECTIONNAME_TURN_LEFT;
        Owner->PlayAnimMontage(TIPMontage, 1.f, SectionName);
    }
}
