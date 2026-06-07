// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionObject/GBInteractableBase.h"
#include "GBItemBase.generated.h"

UCLASS()
class GREEDBOUND_API AGBItemBase : public AGBInteractableBase
{
	GENERATED_BODY()
	
protected:
    virtual TOptional<FPrimaryAssetId>  Interact_Implementation(AActor* Other) override;

private:
    virtual EGBInteractionType          GetInteractionType_Implementation() const override final { return EGBInteractionType::Item; };
   
private:
    UPROPERTY(EditDefaultsOnly)
    FPrimaryAssetId                     ItemDataId;
};
