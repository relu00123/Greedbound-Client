// Fill out your copyright notice in the Description page of Project Settings.

#include "GBDurationIconWidget.h"
#include "GB/Define/GBDefine.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UGBDurationIconWidget::SetIconImage(UTexture2D* Icon)
{
    GB_NULL_CHECK(Img_Icon);
    GB_NULL_CHECK(Img_CoolDownMask);

    Img_Icon->SetBrushFromTexture(Icon);
    Img_CoolDownMask->GetDynamicMaterial()->SetTextureParameterValue(Param_Texture, Icon);
}

void UGBDurationIconWidget::SetIconImageColorAndOpacity(FLinearColor Color)
{
    GB_NULL_CHECK(Img_Icon);

    Img_Icon->SetColorAndOpacity(Color);
}

void UGBDurationIconWidget::SetDurationInfo(const float NewRemainTime, const float NewTotalDuration)
{
    GB_NULL_CHECK(Text_Remain);
    GB_NULL_CHECK(Img_CoolDownMask);

    FNumberFormattingOptions Opts;
    Opts.MaximumFractionalDigits = 1;
    Opts.MinimumFractionalDigits = 1;
    Text_Remain->SetText(FText::AsNumber(NewRemainTime, &Opts));

    const float Percent = FMath::Clamp(NewTotalDuration > 0.f ? (NewRemainTime / NewTotalDuration) : 0.f, 0.f, 1.f);
    Img_CoolDownMask->GetDynamicMaterial()->SetScalarParameterValue(Param_Percent, Percent);
}

void UGBDurationIconWidget::SetRemainTextVisible(const bool bVisible)
{
    Text_Remain->SetRenderOpacity(bVisible ? 1.f : 0.f);
}
