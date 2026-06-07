// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "GBCommonButtonBase.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FGBOnItemAction, UGBCommonButtonBase* ClickedButton);

class UCommonTextBlock;
class UCommonLazyImage;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class GREEDBOUND_API UGBCommonButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void                            SetButtonText(const FText& InButtonText);
    
    UFUNCTION(BlueprintCallable)
    FText                           GetButtonDisplayText() const;
    
    UFUNCTION(BlueprintCallable)
    void                            SetButtonDisplayImage(const FSlateBrush& InBrush);

private:
    virtual void                    NativeOnInitialized() override final;
    virtual void                    NativePreConstruct() override final;
    virtual void                    NativeOnCurrentTextStyleChanged() override final;
    virtual void                    NativeOnHovered() override final;
    virtual void                    NativeOnUnhovered() override final;

    void                            OnItemActionTrigger();

public:
    FGBOnItemAction                   OnItemAction;

private:
    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UCommonTextBlock>    CTB_ButtonText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
    TObjectPtr<UCommonLazyImage>    CLI_ButtonImage;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GB UI Button", meta = (AllowPrivateAccess = "true"))
    FText                           ButtonDisplayText;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GB UI Button", meta = (AllowPrivateAccess = "true"))
    bool                            bUseUpperCaseForButtonText = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GB UI Button", meta = (AllowPrivateAccess = "true"))
    FText                           ButtonDiscriptionText;

    UPROPERTY(EditDefaultsOnly, Category = "PrimaryLayout", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
    FDataTableRowHandle             ItemAction;

    FUIActionBindingHandle          ItemActionHandle;
};
