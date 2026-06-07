// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GBInteractionInterface.generated.h"

class UGBInventoryComponent;
class UGBInteractionComponent;

UINTERFACE(MinimalAPI)
class UGBInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class GREEDBOUND_API IGBInteractionInterface
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    UGBInventoryComponent*      GetInventoryComponent() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    UGBInteractionComponent*    GetInteractionComponment() const;
};
