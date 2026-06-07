// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Define/UI/GBCommonUITypes.h"
#include "Define/UI/GBUIStructTypes.h"
#include "GBListDataObject_Base.generated.h"

#define LIST_DATA_ACCESSOR(DataType, PropertyName) \
    DataType Get##PropertyName() const { return PropertyName; } \
    void Set##PropertyName(DataType In##PropertyName) { PropertyName = In##PropertyName; }

DECLARE_MULTICAST_DELEGATE_TwoParams(FGBOnListDataModifiedDelegate, UGBListDataObject_Base*, EGBOptionsListDataModifyReason);

UCLASS(Abstract)
class GREEDBOUND_API UGBListDataObject_Base : public UObject
{
	GENERATED_BODY()

public:
    LIST_DATA_ACCESSOR(FName, DataID)
    LIST_DATA_ACCESSOR(FText, DataDisplayName)
    LIST_DATA_ACCESSOR(FText, DataDescriptionRichText)
    LIST_DATA_ACCESSOR(FText, DisabledRichText)
    LIST_DATA_ACCESSOR(TSoftObjectPtr<UTexture2D>, SoftDescriptionImage)
    LIST_DATA_ACCESSOR(UGBListDataObject_Base*, ParentData)

    void                                    InitDataObject();

    // 파생클래스 전용
    virtual TArray<UGBListDataObject_Base*> GetAllChildListData() const { return TArray<UGBListDataObject_Base*>(); }
    virtual bool                            HasAnyChildListData() const { return false; }

    // 파생클래스 전용
    virtual bool                            HasDefaultValue() const { return false; }
    virtual bool                            CanResetBackToDefaultValue() const { return false; }
    virtual bool                            TryResetBackToDefaultValue() { return false; }

    void                                    SetShouldApplySettingsImmediately(bool bShouldApplyRightAway) { bShouldApplyChangeImmediatly = bShouldApplyRightAway; }

    void                                    AddEditCondition(const FOptionsDataEditConditionDescriptor& InEditCondition);
    void                                    AddEditDependencyData(UGBListDataObject_Base* InDependencyData);
    bool                                    IsDataCurrentlyEditable();

protected:
    // 파생클래스 전용
    virtual void                            OnDataObjectInitialized() {};
    virtual void                            NotifyListDataModified(UGBListDataObject_Base* InListDataObject, EGBOptionsListDataModifyReason ModifyReason = EGBOptionsListDataModifyReason::DirectlyModified);
    virtual bool                            CanSetToForcedStringValue(const FString& InForcedValue) const { return false; }
    virtual void                            OnSetToForcedStringValue(const FString& InForcedValue) {}
    virtual void                            OnEditDependencyDataModified(UGBListDataObject_Base* ModifiedDependencyData, EGBOptionsListDataModifyReason ModifyReason);

public:
    FGBOnListDataModifiedDelegate           OnListDataModified;
    FGBOnListDataModifiedDelegate           OnDependencyDataModified;

private:
    FName                                   DataID;
    FText                                   DataDisplayName;
    FText                                   DataDescriptionRichText;
    FText                                   DisabledRichText;
    TSoftObjectPtr<UTexture2D>              SoftDescriptionImage;

    UPROPERTY(Transient)
    TObjectPtr<UGBListDataObject_Base>      ParentData;

    // 옵션을 변경한 즉시 적용할 지 여부
    bool                                    bShouldApplyChangeImmediatly = false;

    UPROPERTY(Transient)
    TArray<FOptionsDataEditConditionDescriptor> EditConditionDescList;
};
