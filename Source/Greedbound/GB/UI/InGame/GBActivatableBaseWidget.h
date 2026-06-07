// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Input/UIActionBindingHandle.h"
#include "GBActivatableBaseWidget.generated.h"

/**
 *  Input을 받아야하는 UI창의 BaseClass
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class GREEDBOUND_API UGBActivatableBaseWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
    UGBActivatableBaseWidget(const FObjectInitializer& ObjectInitializer);
    // UI 전용 컨트롤러로 컨트롤러 클래스 교체해야할 수 있음
protected:
    UFUNCTION(BlueprintPure)
    APlayerController*                  GetOwningPlayerController();

    virtual TOptional<FUIInputConfig>   GetDesiredInputConfig() const override;
    virtual void                        NativeOnDeactivated() override;
    
protected:
    UPROPERTY(EditDefaultsOnly, Category = "Activation")
    uint8                               bFocusOnViewportAtDeactivate : 1;

private:
    TWeakObjectPtr<APlayerController>   CachedOwningPC;
};
