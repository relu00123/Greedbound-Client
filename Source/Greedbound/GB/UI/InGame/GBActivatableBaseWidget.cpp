// Fill out your copyright notice in the Description page of Project Settings.

#include "GBActivatableBaseWidget.h"
#include "GameFramework/PlayerController.h"

UGBActivatableBaseWidget::UGBActivatableBaseWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    ActivatedVisibility = ESlateVisibility::SelfHitTestInvisible;
    bIsModal = false;
}

APlayerController* UGBActivatableBaseWidget::GetOwningPlayerController()
{
    if (CachedOwningPC.IsValid() == false)
    {
        CachedOwningPC = GetOwningPlayer<APlayerController>();
    }

    return CachedOwningPC.Get();
}

TOptional<FUIInputConfig> UGBActivatableBaseWidget::GetDesiredInputConfig() const
{
    return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture, EMouseLockMode::DoNotLock, true);
}

void UGBActivatableBaseWidget::NativeOnDeactivated()
{
    Super::NativeOnDeactivated();

    if (bFocusOnViewportAtDeactivate)
    {
        FSlateApplication::Get().SetAllUserFocusToGameViewport();
    }
}
