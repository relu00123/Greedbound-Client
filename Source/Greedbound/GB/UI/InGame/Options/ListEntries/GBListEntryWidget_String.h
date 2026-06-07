// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GB/UI/InGame/Options/ListEntries/GBListEntryWidget_Base.h"
#include "GBListEntryWidget_String.generated.h"

class UGBCommonButtonBase;
class UGBCommonRotator;
class UGBListDataObject_String;

/*
 * 세부옵션 중 선택하는 옵션이 String 타입으로 보여야하는 경우
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class GREEDBOUND_API UGBListEntryWidget_String : public UGBListEntryWidget_Base
{
	GENERATED_BODY()

protected:
    virtual void                            NativeOnInitialized() override final;
    virtual void                            OnOwingListDataObjectSet(UGBListDataObject_Base* InOwningListDataObject) override final;
    virtual void                            OnOwningListDataObjectModified(UGBListDataObject_Base* InListDataObject, EGBOptionsListDataModifyReason ModifyReason) override final;
    virtual void                            OnToggledEditableState(bool bIsEditable) override final;

private:
    void                                    OnPreviousOptionButtonClicked();
    void                                    OnNextOptionButtonClicked();
    void                                    OnRotatorValueChanged(int32 InValue, bool bUserInitialized);

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
    TObjectPtr<UGBCommonButtonBase>         CB_PrevOption;
    
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
    TObjectPtr<UGBCommonButtonBase>         CB_NextOption;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
    TObjectPtr<UGBCommonRotator>            CR_AvaliableOptions;

    UPROPERTY(Transient)
    TObjectPtr<UGBListDataObject_String>    CachedOwingStringDataObject;
};
