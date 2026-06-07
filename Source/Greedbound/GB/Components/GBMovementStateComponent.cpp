// Fill out your copyright notice in the Description page of Project Settings.

#include "GBMovementStateComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Define/GBDefine.h"
#include "AbilitySystem/Attribute/GBSpeedAttributeSet.h"

void UGBMovementStateComponent::BeginPlay()
{
    Super::BeginPlay();

    Movement = GetOwner()->GetComponentByClass<UCharacterMovementComponent>();
}

void UGBMovementStateComponent::OnCurrentSpeedChanged(const FOnAttributeChangeData& Payload)
{
    GB_NULL_CHECK(Movement);

    if (Payload.Attribute == UGBSpeedAttributeSet::GetCurrentSpeedAttribute())
    {
        Movement->MaxWalkSpeed = Payload.NewValue;
    }
}
