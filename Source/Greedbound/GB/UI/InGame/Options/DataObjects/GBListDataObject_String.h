// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GB/UI/InGame/Options/DataObjects/GBListDataObject_Value.h"
#include "GBListDataObject_String.generated.h"

/*
 * ListEntryWidget_String에서 사용하는 OptionList Data
 */
UCLASS()
class GREEDBOUND_API UGBListDataObject_String : public UGBListDataObject_Value
{
	GENERATED_BODY()

public:
    void                    AddDynamicOption(const FString& InStringValue, const FText& InDisplayText);
    void                    AdvanceToNextOption();
    void                    BackToPrevOption();
    void                    OnRotatorInitiatedValueChange(const FText& InNewSelectedText);

    const TArray<FText>&    GetAvailableOptionsTextList() const { return AvailableOptionsTextList; }
    const FText&            GetCurrentDisplayText() const { return CurrentDisplayText; }

protected:
    virtual void            OnDataObjectInitialized() override;
    virtual bool            CanResetBackToDefaultValue() const override final;
    virtual bool            TryResetBackToDefaultValue() override final;

    bool                    TrySetDisplayTextFromStringValue(const FString& InStringValue);
	
protected:
    TArray<FString>         AvailableOptionsStringList;
    TArray<FText>           AvailableOptionsTextList;
    FString                 CurrentStringValue;
    FText                   CurrentDisplayText;
};

UCLASS()
class GREEDBOUND_API UGBListDataObject_StringBool : public UGBListDataObject_String
{
    GENERATED_BODY()

public:
    void            OverrideTrueDisplayText(const FText& InNewTrueDisplayText);
    void            OverrideFalseDisplayText(const FText& InNewFalseDisplayText);
    void            SetTrueAsDefaultValue();
    void            SetFalseAsDefaultValue();

protected:
    virtual void    OnDataObjectInitialized() override final;

private:
    void            TryInitBoolValues();

private:
    const FString   TrueString = TEXT("true");
    const FString   FalseString = TEXT("false");
};

UCLASS()
class GREEDBOUND_API UGBListDataObject_StringEnum : public UGBListDataObject_String
{
    GENERATED_BODY()

public:
    template<typename EnumType>
    void AddEnumOption(EnumType InEnumOption, const FText& InDisplayText)
    {
        const UEnum* StaticEnumOption = StaticEnum<EnumType>();
        const FString ConvertedEnumString = StaticEnumOption->GetNameStringByValue(InEnumOption);

        AddDynamicOption(ConvertedEnumString, InDisplayText);
    }

    template<typename EnumType>
    EnumType GetCurrentValueAsEnum() const
    {
        const UEnum* StaticEnumOption = StaticEnum<EnumType>();
        return static_cast<EnumType>(StaticEnumOption->GetValueByNameString(CurrentStringValue));
    }

    template<typename EnumType>
    void SetDefaultValueFromEnumOption(EnumType InEnumOption)
    {
        const UEnum* StaticEnumOption = StaticEnum<EnumType>();
        const FString ConvertedEnumString = StaticEnumOption->GetNameStringByValue(InEnumOption);

        SetDefaultValueFromString(ConvertedEnumString);
    }
};

UCLASS()
class GREEDBOUND_API UGBListDataObject_StringInteger : public UGBListDataObject_String
{
    GENERATED_BODY()

public:
    void            AddIntegerOption(int32 InIntegerValue, const FText& InDisplayText);

protected:
    virtual void    OnDataObjectInitialized() override final;
    virtual void    OnEditDependencyDataModified(UGBListDataObject_Base* ModifiedDependencyData, EGBOptionsListDataModifyReason ModifyReason) override final;
};
