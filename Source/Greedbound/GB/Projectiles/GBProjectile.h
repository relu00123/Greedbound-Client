// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "GBProjectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class GREEDBOUND_API AGBProjectile : public AActor
{
    GENERATED_BODY()

public:
    AGBProjectile();

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
    TObjectPtr<UProjectileMovementComponent>    ProjectileMovement;

    UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
    FGameplayEffectSpecHandle                   DamageEffectSpecHandle;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ExposeOnSpawn = true))
    float                                       Range;
};
