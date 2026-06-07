// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GBInteractable.generated.h"

enum class EGBInteractionType : uint8;

UINTERFACE(MinimalAPI)
class UGBInteractable : public UInterface
{
	GENERATED_BODY()
};

class GREEDBOUND_API IGBInteractable
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    EGBInteractionType              GetInteractionType() const;
    
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void                            LookAt();
    
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void                            LookAway();
    
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    TOptional<FPrimaryAssetId>      Interact(AActor* Other);
};
