// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GBMonsterBaseInterface.generated.h"

class UGBMonsterBaseData;

UINTERFACE(MinimalAPI)
class UGBMonsterBaseInterface : public UInterface
{
    GENERATED_BODY()
};

class GREEDBOUND_API IGBMonsterBaseInterface
{
    GENERATED_BODY()

public:    
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    UGBMonsterBaseData* GetMonsterBaseData() const;
};
