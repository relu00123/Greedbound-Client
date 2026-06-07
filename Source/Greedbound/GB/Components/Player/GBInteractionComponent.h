// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GBInteractionComponent.generated.h"

class IGBInteractable;
struct FPrimaryAssetId;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGBInteractWithItemDelegate, FPrimaryAssetId, ItemAssetId, int32, Count);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GREEDBOUND_API UGBInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGBInteractionComponent();

    UFUNCTION(BlueprintCallable)
    void                                Interact();

protected:
    virtual void                        TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    AActor*                             FindNearestInteractable();

public:
    FGBInteractWithItemDelegate         OnInteractedWithItem;

private:
    UPROPERTY(Transient)
    TScriptInterface<IGBInteractable>   CurrentFocused;
    
    UPROPERTY(EditDefaultsOnly)
    float                               InteractionDistance     = 300.f;

    UPROPERTY(EditDefaultsOnly)
    float                               InteractionRadius       = 50.f;

    UPROPERTY(EditDefaultsOnly)
    float                               InteractionThreshold    = 70.f;
};
