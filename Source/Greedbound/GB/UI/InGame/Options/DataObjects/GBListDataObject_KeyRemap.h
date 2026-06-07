// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GB/UI/InGame/Options/DataObjects/GBListDataObject_Base.h"
#include "CommonInputTypeEnum.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "GBListDataObject_KeyRemap.generated.h"

class UEnhancedInputUserSettings;
class UEnhancedPlayerMappableKeyProfile;

UCLASS()
class GREEDBOUND_API UGBListDataObject_KeyRemap : public UGBListDataObject_Base
{
	GENERATED_BODY()

public:
    void                                            InitKeyRemapData(UEnhancedInputUserSettings* InOwningInputUserSettings, UEnhancedPlayerMappableKeyProfile* InKeyProfile, ECommonInputType InDesiredInputKeyType, const FPlayerKeyMapping& InOwingPlayerKeyMapping);
    FSlateBrush                                     GetIconFromCurrentKey() const;
    ECommonInputType                                GetDesiredInputKeyType() const { return CachedDesiredInputKeyType; }
    void                                            BindNewInputKey(const FKey& InNewKey);

    virtual bool                                    HasDefaultValue() const override final;
    virtual bool                                    CanResetBackToDefaultValue() const override final;
    virtual bool                                    TryResetBackToDefaultValue() override final;

private:
    FPlayerKeyMapping*                              GetOwningKeyMapping() const;

private:
    UPROPERTY(Transient)
    TObjectPtr<UEnhancedInputUserSettings>          CachedOwingInputUserSettings;
    
	UPROPERTY(Transient)
	TObjectPtr<UEnhancedPlayerMappableKeyProfile>   CachedOwningKeyProfile;

    ECommonInputType                                CachedDesiredInputKeyType;
    FName                                           CachedOwningMappingName;
    EPlayerMappableKeySlot                          CachedOwningMappableKeySlot;
};
