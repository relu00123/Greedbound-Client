// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionObject/GBItemBase.h"
#include "Data/GBItemData.h"
#include "GBWeapon.generated.h"

class UCapsuleComponent;
class UNiagaraComponent;
class UGameplayEffect;
class UWidgetComponent;

UCLASS()
class GREEDBOUND_API AGBWeapon : public AGBItemBase
{
    GENERATED_BODY()

public:    
    AGBWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
    
    const FName&                        GetAttachSocketName() const;
    void                                SetActiveTrailEffect(const bool bActive);

private:
    // InteractableInterface
    virtual TOptional<FPrimaryAssetId>  Interact_Implementation(AActor* Other) override final;

    virtual void                        GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION()
    void                                OnRep_TrailEffectActive();

private:
    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UStaticMeshComponent>    Mesh;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UNiagaraComponent>       TrailParticle;

    UPROPERTY(ReplicatedUsing = OnRep_TrailEffectActive)
    uint8                               bTrailEffectActive : 1;

    UPROPERTY(EditDefaultsOnly)
    FName                               AttachSocketName;
    
    UPROPERTY()
    FGBItemInstanceData                 InstanceData;
};
