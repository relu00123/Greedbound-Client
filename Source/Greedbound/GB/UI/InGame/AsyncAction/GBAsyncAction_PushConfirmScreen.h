// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GBAsyncAction_PushConfirmScreen.generated.h"

enum class EGBConfirmScreenType : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGBOnConfirmScreenButtonClickedDelegate, EGBConfirmScreenButtonType, ClickedButtonType);

UCLASS()
class GREEDBOUND_API UGBAsyncAction_PushConfirmScreen : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject", BlueprintInternalUseOnly = "true", DisplayName = "Show Confirmation Screen"))
    static UGBAsyncAction_PushConfirmScreen*    PushConfirmScreen(const UObject* WorldContextObject, 
                                                                    EGBConfirmScreenType ScreenType, 
                                                                    FText InScreenTitle, 
                                                                    FText InScreenMessage);

    virtual void                                Activate() override final;

public:
    UPROPERTY(BlueprintAssignable)
    FGBOnConfirmScreenButtonClickedDelegate     OnButtonClicked;

private:
    TWeakObjectPtr<UWorld>                      CachedWorld;
    EGBConfirmScreenType                        CachedScreenType;
    FText                                       CachedScreenTitle;
    FText                                       CachedScreenMessage;
};
