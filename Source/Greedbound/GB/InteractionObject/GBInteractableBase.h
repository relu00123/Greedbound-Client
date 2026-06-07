// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/GBInteractable.h"
#include "Define/GBItemTypes.h"
#include "GBInteractableBase.generated.h"

class UWidgetComponent;
class USphereComponent;

UCLASS(Abstract)
class GREEDBOUND_API AGBInteractableBase : public AActor, public IGBInteractable
{
	GENERATED_BODY()
	
public:	
	AGBInteractableBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    // InteractableInterface
    virtual EGBInteractionType          GetInteractionType_Implementation() const override;
    virtual TOptional<FPrimaryAssetId>  Interact_Implementation(AActor* Other) override;

private:
    // InteractableInterface
    virtual void                        LookAt_Implementation() override final;
    virtual void                        LookAway_Implementation() override final;

private:
    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<USphereComponent>        Sphere;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UWidgetComponent>        InteractionWidget;
};
