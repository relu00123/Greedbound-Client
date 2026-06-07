// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GBMovementInterface.generated.h"

class UGBMovementStateComponent;

UINTERFACE(MinimalAPI)
class UGBMovementInterface : public UInterface
{
    GENERATED_BODY()
};

class GREEDBOUND_API IGBMovementInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    UGBMovementStateComponent*  GetMovementStateComponent() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void                        SetRagdoll(bool bRagdoll);
};
