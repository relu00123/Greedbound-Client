// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/GBCharacterStateTypes.h"
#include "GBCombatComponent.generated.h"

class AGBWeapon;
class UGBCombatData;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GREEDBOUND_API UGBCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    void                    EquipWeapon(AGBWeapon* NewWeapon);
    UStaticMeshComponent*   GetWeaponMesh() const;
    void                    SetWeaponTrailEffect(const bool bActive);

    EGBCombatState          GetCombatState() const { return CombatState; }
    void                    SetCombatState(EGBCombatState NewState) { CombatState = NewState; }

    void                    SetAttackProfile(FName Profile) { AttackProfile = Profile; }
    FName                   GetAttackProfile() const { return AttackProfile; }

private:
    UPROPERTY(Transient)
    TObjectPtr<AGBWeapon>   Weapon;

    EGBCombatState          CombatState;
    FName                   AttackProfile;
};
