// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/GBCharacterStateTypes.h"
#include "GBMovementStateComponent.generated.h"

class UAnimMontage;
class UGBAbilitySystemComponent;
class UCharacterMovementComponent;
struct FGameplayTag;
struct FOnAttributeChangeData;

namespace GBMovementConstants
{
    constexpr float MoveThreshold                   = 1.f;
    constexpr float JumpingThreshould               = 10.f;
    constexpr float TurnThresholdForAutonomous      = 60.f;
    constexpr float TurnThresholdForSimulatedProxy  = 10.f;
}

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GREEDBOUND_API UGBMovementStateComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    EGBStanceState                          GetStanceState() const { return StanceState; } 

    UFUNCTION(BlueprintCallable)
    void                                    SetStanceState(EGBStanceState NewState) { StanceState = NewState; }

    UFUNCTION(BlueprintCallable)
    EGBMoveState                            GetMoveState() const { return MoveState; }

    UFUNCTION(BlueprintCallable)
    void                                    SetMoveState(EGBMoveState NewState) { MoveState = NewState; }

    UFUNCTION(BlueprintCallable)
    UAnimMontage*                           GetTurnInPlaceMontage() const { return TurnInPlaceMontage; }

    void                                    OnCurrentSpeedChanged(const FOnAttributeChangeData& Payload);

private:
    virtual void                            BeginPlay() override final;

private:
    UPROPERTY(Transient)
    TObjectPtr<UCharacterMovementComponent> Movement;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UAnimMontage>                TurnInPlaceMontage;

    EGBStanceState                          StanceState     = EGBStanceState::Stand;
    EGBMoveState                            MoveState       = EGBMoveState::Walk;
};
