// Fill out your copyright notice in the Description page of Project Settings.


#include "GBProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

AGBProjectile::AGBProjectile()
{
    bReplicates = true;

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
}
