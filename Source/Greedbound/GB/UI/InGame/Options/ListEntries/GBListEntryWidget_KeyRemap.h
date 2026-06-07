// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GB/UI/InGame/Options/ListEntries/GBListEntryWidget_Base.h"
#include "GBListEntryWidget_KeyRemap.generated.h"

class UGBCommonButtonBase;
class UGBListDataObject_KeyRemap;

UCLASS()
class GREEDBOUND_API UGBListEntryWidget_KeyRemap : public UGBListEntryWidget_Base
{
	GENERATED_BODY()

protected:
    virtual void                            NativeOnInitialized() override final;

    virtual void                            OnOwingListDataObjectSet(UGBListDataObject_Base* InOwningListDataObject) override final;
    virtual void                            OnOwningListDataObjectModified(UGBListDataObject_Base* InListDataObject, EGBOptionsListDataModifyReason ModifyReason) override final;

private:
    void                                    OnRemapKeyButtonClicked();
    void                                    OnResetKeyBindingButtonClicked();

    void                                    OnKeyToRemapPressed(const FKey& PressedKey);
    void                                    OnKeyRemapCanceled(const FString& CanceledReason);
	
private:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
    TObjectPtr<UGBCommonButtonBase>         CB_RemapKey;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
    TObjectPtr<UGBCommonButtonBase>         CB_ResetKeyBinding;

    UPROPERTY(Transient)
    TObjectPtr<UGBListDataObject_KeyRemap>  CachedOwningKeyRemapDataObject;
};
