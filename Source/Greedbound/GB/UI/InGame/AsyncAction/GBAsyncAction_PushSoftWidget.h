// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GameplayTagContainer.h"
#include "GBAsyncAction_PushSoftWidget.generated.h"

class UGBActivatableBaseWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGBOnPushSoftWidgetDelegate, UGBActivatableBaseWidget*, PushedWidget);

UCLASS()
class GREEDBOUND_API UGBAsyncAction_PushSoftWidget : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject", BlueprintInternalUseOnly = "true", DisplayName = "Push Soft Widget To Widget Stack"))
    static UGBAsyncAction_PushSoftWidget* PushSoftWidget(const UObject* WorldContextObject, APlayerController* OwningPlayerController, 
                                                        TSoftClassPtr<UGBActivatableBaseWidget> InSoftWidgetClass, 
                                                        UPARAM(meta = (Categories = "UI.WidgetStack")) FGameplayTag InWidgetStackTag, 
                                                        bool bFocusOnNewlyPushedWidget = true);

    virtual void                                Activate() override final;

public:
    UPROPERTY(BlueprintAssignable)
    FGBOnPushSoftWidgetDelegate                 OnWidgetCreatedBeforePush;

    UPROPERTY(BlueprintAssignable)
    FGBOnPushSoftWidgetDelegate                 OnWidgetCreatedAfterPush;

private:
    TWeakObjectPtr<UWorld>                      CachedWorld;
    TWeakObjectPtr<APlayerController>           CachedOwningPC;
    TSoftClassPtr<UGBActivatableBaseWidget>     CachedSoftWidgetClass;
    FGameplayTag                                CachedWidgetStackTag;
    bool                                        bCachedFocusOnNewlyPushedWidget = false;
};
