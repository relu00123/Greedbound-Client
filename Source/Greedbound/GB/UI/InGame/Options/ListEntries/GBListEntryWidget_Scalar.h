// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GB/UI/InGame/Options/ListEntries/GBListEntryWidget_Base.h"
#include "GBListEntryWidget_Scalar.generated.h"

class UCommonNumericTextBlock;
class UAnalogSlider;
class UGBListDataObject_Scalar;

/*
 * 세부옵션 중 선택하는 옵션이 Scalar 타입으로 보여야하는 경우 ex) 볼륨
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class GREEDBOUND_API UGBListEntryWidget_Scalar : public UGBListEntryWidget_Base
{
	GENERATED_BODY()

protected:
    virtual void                            NativeOnInitialized() override final;
    virtual void                            OnOwingListDataObjectSet(UGBListDataObject_Base* InOwningListDataObject) override final;
    virtual void                            OnOwningListDataObjectModified(UGBListDataObject_Base* InListDataObject, EGBOptionsListDataModifyReason ModifyReason) override final;

private:
    UFUNCTION()
    void                                    OnSliderValueChanged(float InValue);

    UFUNCTION()
    void                                    OnSliderMouseCaptureBegin();
	
private:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
    TObjectPtr<UCommonNumericTextBlock>     CNTB_SettingValue;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
    TObjectPtr<UAnalogSlider>               AS_SettingSlider;

    UPROPERTY(Transient)
    TObjectPtr<UGBListDataObject_Scalar>    CachedOwningScalarDataObject;
};
