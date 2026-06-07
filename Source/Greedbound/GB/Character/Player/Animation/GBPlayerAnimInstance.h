// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/GBCharacterStateTypes.h"
#include "GBPlayerAnimInstance.generated.h"

class ACharacter;
class UCharacterMovementComponent;
class UGBMovementStateComponent;
class UGBCombatComponent;

UCLASS()
class GREEDBOUND_API UGBPlayerAnimInstance : public UAnimInstance
{
    GENERATED_BODY()
    
protected:
    virtual void                            NativeBeginPlay() override;
    virtual void                            NativeUpdateAnimation(float DeltaSeconds) override;

private:
    void                                    UpdateTurnInPlace();

protected:
    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    TObjectPtr<ACharacter>                  Owner;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    TObjectPtr<UCharacterMovementComponent> Movement;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    TObjectPtr<UGBMovementStateComponent>   MovementState;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    TObjectPtr<UGBCombatComponent>          Combat;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    FVector                                 Velocity;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    FRotator                                LastRotation;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    FRotator                                CurrentRotation;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    FRotator                                DeltaRotation;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float                                   GroundSpeed;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float                                   JumpingThreshould = 10.f;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float                                   Forward;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float                                   Strafe;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float                                   CrouchSpeedCoefficient = 0.6f;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float                                   BaseWalkSpeed = 300.f;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float                                   BaseSprintSpeed = 500.f;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    EGBStanceState                          StanceState;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    EGBStopState                            StopState;
    
    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    EGBMoveState                            MoveState;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    bool                                    bIsFalling;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    bool                                    bIsJumping;
};
