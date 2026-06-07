// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GB/UI/InGame/GBActivatableBaseWidget.h"
#include "GBOptionsScreenWidget.generated.h"

class UGBOptionsDataRegistry;
class UGBTabListWidgetBase;
class UGBCommonListView;
class UGBOptionsDetailsViewWidget;
class UGBListDataObject_Base;
enum class EGBOptionsListDataModifyReason : uint8;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class GREEDBOUND_API UGBOptionsScreenWidget : public UGBActivatableBaseWidget
{
	GENERATED_BODY()
	
protected:
    // UserWidget Interface
    virtual void                                NativeOnInitialized() override final;

    // CommonActivatableWidget Interface
    virtual void                                NativeOnActivated() override final;
    virtual void                                NativeOnDeactivated() override final;
    virtual UWidget*                            NativeGetDesiredFocusTarget() const override final;

private:
    void                                        OnResetBoundActionTriggered();

    UGBOptionsDataRegistry*                     GetOrCreateDataRegistry();

    UFUNCTION()
    void                                        OnOptionsTabSelected(FName TabId);

    void                                        OnListViewItemHovered(UObject* InHoveredItem, bool bWasHovered);
    void                                        OnListViewItemSelected(UObject* InSelectedItem);

    FString                                     TryGetEntryWidgetClassName(UObject* InOwningListItem) const;

    void                                        OnListViewListDataModified(UGBListDataObject_Base* InDataObject, EGBOptionsListDataModifyReason ModifyReason);

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGBTabListWidgetBase>            CTL_OptionsTabListWidget;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGBCommonListView>               CLV_OptionList;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGBOptionsDetailsViewWidget>     DetailsView_ListEntryInfo;

    /// Options Screen에서 만들어 반환하는 Options Data Registry, 직접 접근 금지
    UPROPERTY(Transient)
    TObjectPtr<UGBOptionsDataRegistry>          OptionsDataRegistry;

	UPROPERTY(EditDefaultsOnly, Category = "Options", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
    FDataTableRowHandle                         ResetAction;

    FUIActionBindingHandle                      ResetActionHandle;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UGBListDataObject_Base>>  ResetableDataList;

    bool                                        bIsResettingData = false;
};
