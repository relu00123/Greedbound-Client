// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Delegates/Delegate.h"
#include "Delegates/DelegateCombinations.h"
#include "GBOutsideClickCatcherWidget.generated.h"

class UButton;
class UCanvasPanel;

UCLASS()
class GREEDBOUND_API UGBOutsideClickCatcherWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void                        BindOnClicked(const FSimpleDelegate& Callback);

    FORCEINLINE UCanvasPanel*   GetRootCanvas() const { return RootCanvas; }

protected:
    virtual void                NativeConstruct() override;
    virtual FReply              NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCanvasPanel>    RootCanvas;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton>         OutsideClickCatcherBtn;

    FSimpleDelegate             OnClickedCallback;

    UFUNCTION()
    void                        HandleClick();
};
