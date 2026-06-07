// Fill out your copyright notice in the Description page of Project Settings.

#include "GBDataListEntryMappingData.h"
#include "Define/GBDefine.h"
#include "UI/InGame/Options/DataObjects/GBListDataObject_Base.h"

TSubclassOf<UGBListEntryWidget_Base> UGBDataListEntryMappingData::FindEntryWidgetClassByDataObject(UGBListDataObject_Base* InDataObject) const
{
    GB_NULL_CHECK_WITH_RETURN(InDataObject, TSubclassOf<UGBListEntryWidget_Base>());

    UClass* DataObjectClass = InDataObject->GetClass();
    for (; DataObjectClass; DataObjectClass = DataObjectClass->GetSuperClass())
    {
        if (TSubclassOf<UGBListDataObject_Base> ConvertedDataObjectClass = TSubclassOf<UGBListDataObject_Base>(DataObjectClass))
        {
            if (DataObjectListEntryMap.Contains(ConvertedDataObjectClass))
            {
                return DataObjectListEntryMap.FindRef(ConvertedDataObjectClass);
            }
        }
    }

    return TSubclassOf<UGBListEntryWidget_Base>();
}
