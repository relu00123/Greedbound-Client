// Fill out your copyright notice in the Description page of Project Settings.

#include "GBCombatComponent.h"
#include "Define/GBDefine.h"
#include "InteractionObject/Weapon/GBWeapon.h"

void UGBCombatComponent::EquipWeapon(AGBWeapon* NewWeapon)
{
    GB_NULL_CHECK(NewWeapon);
    GB_VALID_CHECK(GetOwner());

    if (GetOwner()->HasAuthority() == false)
    {
        return;
    }

    if (Weapon)
    {
        Weapon->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
        Weapon->SetOwner(nullptr);
        Weapon = nullptr;
    }

    AActor* OwningActor = GetOwner();
    GB_NULL_CHECK(OwningActor);

    Weapon = NewWeapon;
    Weapon->SetOwner(OwningActor);

    USkeletalMeshComponent* SkeletalMesh = OwningActor->GetComponentByClass<USkeletalMeshComponent>();
    GB_NULL_CHECK(SkeletalMesh);

    Weapon->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, Weapon->GetAttachSocketName());
}

UStaticMeshComponent* UGBCombatComponent::GetWeaponMesh() const
{
    GB_VALID_CHECK_WITH_RETURN(Weapon, nullptr);

    return Weapon->GetComponentByClass<UStaticMeshComponent>();
}

void UGBCombatComponent::SetWeaponTrailEffect(const bool bActive)
{
    GB_VALID_CHECK(Weapon);

    Weapon->SetActiveTrailEffect(bActive);
}
