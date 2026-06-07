// Fill out your copyright notice in the Description page of Project Settings.


#include "GB/UI/Popup/Utils/GBOutsideClickCatcherWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"

void UGBOutsideClickCatcherWidget::BindOnClicked(const FSimpleDelegate& Callback)
{
    OnClickedCallback = Callback;
}

void UGBOutsideClickCatcherWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (OutsideClickCatcherBtn)
    {
        //OutsideClickCatcherBtn->IsFocusable = false;
        OutsideClickCatcherBtn->SetClickMethod(EButtonClickMethod::MouseDown);
        OutsideClickCatcherBtn->OnClicked.AddDynamic(this, &UGBOutsideClickCatcherWidget::HandleClick);
    }
}

FReply UGBOutsideClickCatcherWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // 좌클릭 또는 우클릭 모두 허용
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton ||
        InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        OnClickedCallback.ExecuteIfBound(); // 내부에 바인딩된 FSimpleDelegate 호출
        return FReply::Handled();
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}


void UGBOutsideClickCatcherWidget::HandleClick()
{
    if (OnClickedCallback.IsBound())
    {
        OnClickedCallback.Execute();
    }
}
