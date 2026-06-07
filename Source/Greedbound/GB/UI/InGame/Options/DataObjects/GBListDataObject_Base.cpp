// Fill out your copyright notice in the Description page of Project Settings.

#include "GBListDataObject_Base.h"
#include "ProjectSetting/GBGameUserSettings.h"
#include "Define/GBDefine.h"

void UGBListDataObject_Base::InitDataObject()
{
    OnDataObjectInitialized();
}

void UGBListDataObject_Base::AddEditCondition(const FOptionsDataEditConditionDescriptor& InEditCondition)
{
    EditConditionDescList.Add(InEditCondition);
}

void UGBListDataObject_Base::AddEditDependencyData(UGBListDataObject_Base* InDependencyData)
{
    GB_NULL_CHECK(InDependencyData);

    if (InDependencyData->OnListDataModified.IsBoundToObject(this) == false)
    {
        InDependencyData->OnListDataModified.AddUObject(this, &UGBListDataObject_Base::OnEditDependencyDataModified);
    }
}

bool UGBListDataObject_Base::IsDataCurrentlyEditable()
{
    bool bIsEditable = true;

    if (EditConditionDescList.IsEmpty())
    {
        return bIsEditable;
    }

    FString CachedDisabledRichReason;

    for (const FOptionsDataEditConditionDescriptor& Condition : EditConditionDescList)
    {
        if (Condition.IsValid() == false || Condition.IsEditConditionMet())
        {
            continue;
        }

        bIsEditable = false;

        CachedDisabledRichReason.Append(Condition.GetDisableRichReason());
        SetDisabledRichText(FText::FromString(CachedDisabledRichReason));

        if (Condition.HasForcedStringValue())
        {
            const FString& ForcedStringValue = Condition.GetDisableForcedStringValue();

            if (CanSetToForcedStringValue(ForcedStringValue))
            {
                OnSetToForcedStringValue(ForcedStringValue);
            }
        }
    }

    return bIsEditable;
}

void UGBListDataObject_Base::NotifyListDataModified(UGBListDataObject_Base* InListDataObject, EGBOptionsListDataModifyReason ModifyReason /*= EGBOptionsListDataModifyReason::DirectlyModified*/)
{
    OnListDataModified.Broadcast(InListDataObject, ModifyReason);

    if (bShouldApplyChangeImmediatly)
    {
        UGBGameUserSettings::Get()->ApplySettings(true);
    }
}

void UGBListDataObject_Base::OnEditDependencyDataModified(UGBListDataObject_Base* ModifiedDependencyData, EGBOptionsListDataModifyReason ModifyReason)
{
    OnDependencyDataModified.Broadcast(ModifiedDependencyData, ModifyReason);
}
