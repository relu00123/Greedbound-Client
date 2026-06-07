// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GBLobbyTabButton.generated.h"

class UImage;
class UTextBlock;
class UButton;

UCLASS()
class GREEDBOUND_API UGBLobbyTabButton : public UUserWidget
{
    GENERATED_BODY()
    
public:
    UFUNCTION(BlueprintCallable)
    void                    SetFocusDecorationVisible(bool bVisible);

protected:
    virtual void            NativePreConstruct() override;
    
public:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage>      UpperSelectedDecorationImage;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock>  ButtonText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton>     TabButton;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CustomVariables")
    FText                   DesignerButtonText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignerOnly")
    bool                    bUpperDecorationVisible = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tab | TextColor")
    FSlateColor             NormalColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tab | TextColor")
    FSlateColor             ClickedColor;
};
