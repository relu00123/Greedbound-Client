// Fill out your copyright notice in the Description page of Project Settings.


#include "GB/UI/Lobby/GBLobbyTabButton.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Styling/SlateColor.h"

void UGBLobbyTabButton::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (UpperSelectedDecorationImage)
    {
        SetFocusDecorationVisible(bUpperDecorationVisible);
    }

    if (ButtonText && !DesignerButtonText.IsEmpty()) {
        ButtonText->SetText(DesignerButtonText);
    }
}

void UGBLobbyTabButton::SetFocusDecorationVisible(bool bVisible)
{
    if (UpperSelectedDecorationImage)
    {
        UpperSelectedDecorationImage->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }

    if (ButtonText)
    {
        ButtonText->SetColorAndOpacity(bVisible ? ClickedColor : NormalColor);
    }
}
