// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GB/UI/InGame/Options/DataObjects/GBListDataObject_String.h"
#include "GBListDataObject_StrResolution.generated.h"

/**
 * 해상도 지정 OptionUI용 DataObject
 */
UCLASS()
class GREEDBOUND_API UGBListDataObject_StrResolution : public UGBListDataObject_String
{
	GENERATED_BODY()

public:
    void            InitResolutionValues();
    FString         GetMaxinumAllowedResolution() const { return MaximumAllowedResolution; }

protected:
    virtual void    OnDataObjectInitialized() override final; 
    virtual bool    CanSetToForcedStringValue(const FString& InForcedValue) const override final;
    virtual void    OnSetToForcedStringValue(const FString& InForcedValue) override final;


private:
    FString         ResToValueString(const FIntPoint& InResolution) const;
    FText           ResToDisplayText(const FIntPoint& InResolution) const;

private:
    FString         MaximumAllowedResolution;
};
