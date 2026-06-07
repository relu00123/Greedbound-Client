// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractionObject/GBItemBase.h"

TOptional<FPrimaryAssetId> AGBItemBase::Interact_Implementation(AActor* Other)
{
    Super::Interact_Implementation(Other);

    return TOptional<FPrimaryAssetId>(ItemDataId);
}
