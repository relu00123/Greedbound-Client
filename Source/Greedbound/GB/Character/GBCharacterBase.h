// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interface/GBCombatInterface.h"
#include "Interface/GBMovementInterface.h"
#include "GBCharacterBase.generated.h"

class UAbilitySystemComponent;
class UGBAbilitySystemComponent;
class UGBMovementStateComponent;
class UGBCombatComponent;
struct FGameplayEventData;

UCLASS(Abstract)
class GREEDBOUND_API AGBCharacterBase : public ACharacter, public IAbilitySystemInterface, public IGBCombatInterface, public IGBMovementInterface
{
    GENERATED_BODY()

public:
    AGBCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    virtual void                            BeginPlay() override;
    virtual void                            Tick(float DeltaTime) override;
    virtual void                            PossessedBy(AController* NewController) override;

    virtual UAbilitySystemComponent*        GetAbilitySystemComponent() const override final;

    template <typename T>
    T* GetAbilitySystemComponent() const { return Cast<T>(GetAbilitySystemComponent()); }

    void                                    SetAbilitySystemComponent(UGBAbilitySystemComponent* ASC);
    void                                    InitAbilitySystemComponent(AActor* OwnerActor);

    // IGBCombatInterface
    virtual UGBCombatComponent*             GetCombatComponent_Implementation() const override { return CombatComponent; }

    // IGBMovementInterface
    virtual UGBMovementStateComponent*      GetMovementStateComponent_Implementation() const override { return MovementStateComponent; }
    virtual void                            SetRagdoll_Implementation(bool bRagdoll) override;

    virtual void                            LoadCharacterData();

private:
    UFUNCTION(NetMulticast, Reliable)
    void                                    Multicast_BeRagdoll(bool bActive);

private:
    UPROPERTY(Transient)
    TObjectPtr<UGBAbilitySystemComponent>   AbilitySystemComponent;

    UPROPERTY(EditDefaultsOnly, Category = "BaseCharacter")
    TObjectPtr<UGBMovementStateComponent>   MovementStateComponent;

    UPROPERTY(EditDefaultsOnly, Category = "BaseCharacter")
    TObjectPtr<UGBCombatComponent>          CombatComponent;

    UPROPERTY(EditDefaultsOnly, Category = "BaseCharacter", meta = (AllowedTypes = "CharacterData"))
    FPrimaryAssetId                         CharacterDataAssetId;
};
