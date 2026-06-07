// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GBOptionsDetailsViewWidget.generated.h"

class UCommonTextBlock;
class UCommonLazyImage;
class UCommonRichTextBlock;
class UGBListDataObject_Base;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class GREEDBOUND_API UGBOptionsDetailsViewWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    void                                UpdateDetailsViewInfo(UGBListDataObject_Base* InDataObject, const FString& InEntryWidgetClassName = FString());
    void                                ClearDetailsViewInfo();

protected:
    virtual void                        NativeOnInitialized() override final;
	
private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonTextBlock>        CTB_Title;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonLazyImage>        CLI_DescriptionImage;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonRichTextBlock>    CRT_Description;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonRichTextBlock>    CRT_DynamicDetails;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCommonRichTextBlock>    CRT_DisableReason;
};
