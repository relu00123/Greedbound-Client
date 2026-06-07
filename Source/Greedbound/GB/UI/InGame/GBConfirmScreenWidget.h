// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/InGame/GBActivatableBaseWidget.h"
#include "Define/UI/GBCommonUITypes.h"
#include "GBConfirmScreenWidget.generated.h"

class UCommonTextBlock;
class UDynamicEntryBox;

USTRUCT(BlueprintType)
struct FGBConfirmScreenButtonInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EGBConfirmScreenButtonType  ButtonType = EGBConfirmScreenButtonType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText                       ButtonTextToDisplay;
};

UCLASS()
class GREEDBOUND_API UGBConfirmScreenInfoObject : public UGBActivatableBaseWidget
{
    GENERATED_BODY()

public:
    static UGBConfirmScreenInfoObject*  CreateOkScreenInfoObject(const FText& InScreenTitle, const FText& InScreenMessage);
    static UGBConfirmScreenInfoObject*  CreateYesNoScreenInfoObject(const FText& InScreenTitle, const FText& InScreenMessage);
    static UGBConfirmScreenInfoObject*  CreateOkCancelScreenInfoObject(const FText& InScreenTitle, const FText& InScreenMessage);

public:
    UPROPERTY(Transient)
    FText                               ScreenTitle;

    UPROPERTY(Transient)
    FText                               ScreenMessage;

    UPROPERTY(Transient)
    TArray<FGBConfirmScreenButtonInfo>  AvaliableScreenButtons;
};

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class GREEDBOUND_API UGBConfirmScreenWidget : public UGBActivatableBaseWidget
{
	GENERATED_BODY()

public:
    void                                InitConfirmScreen(UGBConfirmScreenInfoObject* InScreenInfoObject, TFunction<void(EGBConfirmScreenButtonType)> ClickedButtonCallback);
    virtual UWidget*                    NativeGetDesiredFocusTarget() const override final;
	
private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonTextBlock>        CTB_Title;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonTextBlock>        CTB_Message;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UDynamicEntryBox>        DEB_Buttons;
};
