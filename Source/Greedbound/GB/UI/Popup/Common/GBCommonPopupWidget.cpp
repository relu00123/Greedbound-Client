// Fill out your copyright notice in the Description page of Project Settings.

#include "GB/UI/Popup/Common/GBCommonPopupWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"

FVector2D DragStartPosition;
FVector2D WidgetStartOffset;

void UGBCommonPopupWidget::SetupPopup(const FGBPopupConfig& Config)
{
    StoredConfig = Config;

    this->SetVisibility(ESlateVisibility::Visible);
    MessageText->SetText(FText::FromString(Config.Message));

    //ConfirmFn = Config.OnConfirm;
    //CancelFn = Config.OnCancel;
    //OnTimeOut = Config.OnTimeOut;
    CancelButton->SetVisibility(Config.bUseCancel ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

    if (StoredConfig.bUseTimeout && RemainingTimeProgressBar)
    {
        TotalTime = FMath::Max(0.f, StoredConfig.TimeoutSeconds);
        RemainingTime = TotalTime;

        RemainingTimeProgressBar->SetVisibility(ESlateVisibility::Visible);
        RemainingTimeProgressBar->SetPercent(1.0f);

        // 타이머 시작 (0.01초 간격으로 Update호출)
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle_ProgressBar,
            this,
            &UGBCommonPopupWidget::UpdateProgressBar,
            0.01f,
            true
        );
    }

    else
    {
        RemainingTimeProgressBar->SetVisibility(ESlateVisibility::Collapsed);
    }
}

FDelegateHandle UGBCommonPopupWidget::BindOnConfirmed(const FSimpleDelegate& InDelegate)
{
    return OnConfirmed.Add(InDelegate);
}

void UGBCommonPopupWidget::UnBindOnConfirmed(FDelegateHandle Handle)
{
    OnConfirmed.Remove(Handle);
}

FDelegateHandle UGBCommonPopupWidget::BindOnCanceled(const FSimpleDelegate& InDelegate)
{
    return OnCanceled.Add(InDelegate);
}

void UGBCommonPopupWidget::UnBindOnCanceled(FDelegateHandle Handle)
{
    OnCanceled.Remove(Handle);
}

FDelegateHandle UGBCommonPopupWidget::BindOnTimedOut(const FSimpleDelegate& InDelegate)
{
    return OnTimedOut.Add(InDelegate);
}

void UGBCommonPopupWidget::UnBindOnTimedOut(FDelegateHandle Handle)
{
    OnTimedOut.Remove(Handle);
}

FReply UGBCommonPopupWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && TargetSizeBox)
    {
        DragStartPosition = InMouseEvent.GetScreenSpacePosition();
        WidgetStartOffset = TargetSizeBox->GetRenderTransform().Translation;
        bIsDragging = true;

        //  this->TakeWidget()으로 해도 CaptureMouse는 작동하지 않을 수 있음
        // 하지만 FReply::Handled()만으로도 드래그는 대부분 동작함
        return FReply::Handled();
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UGBCommonPopupWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (bIsDragging && TargetSizeBox)
    {
        const FVector2D CurrentPosition = InMouseEvent.GetScreenSpacePosition();
        const FVector2D Delta = CurrentPosition - DragStartPosition;

        TargetSizeBox->SetRenderTranslation(WidgetStartOffset + Delta);

        return FReply::Handled();
    }

    return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

FReply UGBCommonPopupWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        bIsDragging = false;
        return FReply::Handled();
    }

    return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UGBCommonPopupWidget::UpdateProgressBar()
{
    RemainingTime -= 0.01f;

    // 비율 계산
    float Ratio = FMath::Clamp(RemainingTime / TotalTime, 0.0f, 1.0f);
    RemainingTimeProgressBar->SetPercent(Ratio);

    // 시간 초과 시 닫기
    if (RemainingTime <= 0.0f)
    {
        GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ProgressBar);

        OnTimedOut.Broadcast();

        RemoveFromParent();
    }
}


void UGBCommonPopupWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ConfirmButton->OnClicked.AddUniqueDynamic(this, &UGBCommonPopupWidget::OnConfirmClicked);
    CancelButton->OnClicked.AddUniqueDynamic(this, &UGBCommonPopupWidget::OnCancelClicked);
}

void UGBCommonPopupWidget::OnConfirmClicked()
{
    OnConfirmed.Broadcast();
    RemoveFromParent();
     
}

void UGBCommonPopupWidget::OnCancelClicked()
{
    OnCanceled.Broadcast();
    RemoveFromParent();
}
