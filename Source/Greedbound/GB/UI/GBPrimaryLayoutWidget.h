// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI\InGame\GBActivatableBaseWidget.h"
#include "Input/UIActionBindingHandle.h"
#include "GameplayTagContainer.h"
#include "GBPrimaryLayoutWidget.generated.h"

class UCommonActivatableWidgetContainerBase;
class UAbilitySystemComponent;
class UGBPlayerHUDWidget;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class GREEDBOUND_API UGBPrimaryLayoutWidget : public UGBActivatableBaseWidget
{
    GENERATED_BODY()
public:
    UCommonActivatableWidgetContainerBase*  FindWidgetStackByTag(const FGameplayTag& InTag) const;
    void                                    SetHUDVisible(bool bVisibie);
    void                                    SetHUDWidget(UAbilitySystemComponent* InASC);

protected:
    UFUNCTION(BlueprintCallable)
    void                                    RegisterWidgetStack(UPARAM(meta = (Categories = "UI.WidgetStack"))FGameplayTag InTag, UWidget* InWidgetStack);

private:
    virtual TOptional<FUIInputConfig>       GetDesiredInputConfig() const override final;
    virtual void                            NativeOnInitialized() override final;
    virtual void                            NativeOnActivated() override final;
    virtual bool                            NativeOnHandleBackAction() override final;

    void                                    OnInventoryActionTriggered();

    FGameplayTag                            GetSkillTagByIndex(uint8 Index) const;

private:
    UPROPERTY(Transient)
    TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>>   RegisteredWidgetStackMap;

	UPROPERTY(EditDefaultsOnly, Category = "PrimaryLayout", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
    FDataTableRowHandle                     InventoryAction;

    UPROPERTY(Transient)
    TObjectPtr<UGBPlayerHUDWidget>          HUDWidget;

    UPROPERTY(EditDefaultsOnly, Category = "PrimaryLayout")
    TSubclassOf<UGBPlayerHUDWidget>         HUDWidgetClass;
};
