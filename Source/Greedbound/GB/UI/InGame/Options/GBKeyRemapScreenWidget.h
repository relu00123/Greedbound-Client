// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GB/UI/InGame/GBActivatableBaseWidget.h"
#include "CommonInputTypeEnum.h"
#include "GBKeyRemapScreenWidget.generated.h"

class UCommonRichTextBlock;
class FGBKeyRemapScreenInputPreprocessor;

DECLARE_DELEGATE_OneParam(FGBOnKeyRemapScreenKeyPressedDelegate, const FKey& /*PressedKey*/)
DECLARE_DELEGATE_OneParam(FGBOnKeyRemapScreenKeySelectedCanceledDelegate, const FString& /*CanceledReason*/)

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class GREEDBOUND_API UGBKeyRemapScreenWidget : public UGBActivatableBaseWidget
{
	GENERATED_BODY()

public:
    void                                            SetDesiredInputTypeToFilter(ECommonInputType InDesiredInputType);

protected:
    virtual void                                    NativeOnActivated() override final;
    virtual void                                    NativeOnDeactivated() override final;

private:
    void                                            OnValidKeyPressedDetected(const FKey& PressedKey);
    void                                            OnKeySelectedCanceled(const FString & CanceledReason);

    void                                            RequestDeactivateWidget(TFunction<void()> PreDeactivateCallback);

public:
    FGBOnKeyRemapScreenKeyPressedDelegate           OnKeyRemapScreenKeyPressed;
    FGBOnKeyRemapScreenKeySelectedCanceledDelegate  OnKeyRemapScreenKeySelectedCanceled;

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonRichTextBlock>                CRT_RemapMessage;

    TSharedPtr<FGBKeyRemapScreenInputPreprocessor>  CachedInputPreprecessor;
    ECommonInputType                                CachedDesiredInputType;
};
