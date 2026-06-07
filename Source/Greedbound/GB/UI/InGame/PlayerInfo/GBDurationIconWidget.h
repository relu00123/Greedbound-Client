// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GBDurationIconWidget.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class GREEDBOUND_API UGBDurationIconWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void                    SetIconImage(UTexture2D* Icon);
    void                    SetIconImageColorAndOpacity(FLinearColor Color);
    void                    SetDurationInfo(const float NewRemainTime, const float NewTotalDuration);
    void                    SetRemainTextVisible(const bool bVisible);
    
private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage>      Img_Icon;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage>      Img_CoolDownMask;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock>  Text_Remain;

    float                   RemainTime      = 0.f;    
    float                   TotalDuration   = 0.f;

    FName                   Param_Texture   = FName("Icon");
    FName                   Param_Percent   = FName("Percent");
};
