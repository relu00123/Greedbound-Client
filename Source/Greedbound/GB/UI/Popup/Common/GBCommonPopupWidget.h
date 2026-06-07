// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Popup/Utils/GBPopupConfig.h"
#include "GBCommonPopupWidget.generated.h"

class UTextBlock;
class UButton;
class USizeBox;
class UProgressBar;

DECLARE_MULTICAST_DELEGATE(FGBPopupSimpleEvent);

UCLASS()
class GREEDBOUND_API UGBCommonPopupWidget : public UUserWidget
{
    GENERATED_BODY()
    
public:
    void                        SetupPopup(const FGBPopupConfig& Config);

    FGBPopupConfig&             GetConfigRef() { return StoredConfig; }

    FDelegateHandle             BindOnConfirmed(const FSimpleDelegate& InDelegate);
    void                        UnBindOnConfirmed(FDelegateHandle Handle); 

    FDelegateHandle             BindOnCanceled(const FSimpleDelegate& InDelegate);
    void                        UnBindOnCanceled(FDelegateHandle Handle);

    FDelegateHandle             BindOnTimedOut(const FSimpleDelegate& InDelegate);
    void                        UnBindOnTimedOut(FDelegateHandle Handle);
 
protected:
    virtual void                NativeConstruct() override; 

    virtual FReply              NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply              NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply              NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    void                        UpdateProgressBar();

private:
    UFUNCTION()
    void                        OnConfirmClicked();

    UFUNCTION() 
    void                        OnCancelClicked();

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock>      MessageText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton>         ConfirmButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton>         CancelButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USizeBox>        TargetSizeBox;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar>    RemainingTimeProgressBar;

private:
    /*TFunction<void()>            ConfirmFn;
    TFunction<void()>            CancelFn;
    TFunction<void()>            OnTimeOut;*/

    bool                        bIsDragging; 

    float                       RemainingTime = 0.0f;
    float                       TotalTime = 0.0f; 
    FTimerHandle                TimerHandle_ProgressBar;

    FGBPopupConfig              StoredConfig;

    FGBPopupSimpleEvent         OnConfirmed;
    FGBPopupSimpleEvent         OnCanceled;
    FGBPopupSimpleEvent         OnTimedOut; 
};
