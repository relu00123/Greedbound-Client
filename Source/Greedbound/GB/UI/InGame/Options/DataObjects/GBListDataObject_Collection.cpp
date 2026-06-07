// Fill out your copyright notice in the Description page of Project Settings.

#include "GBListDataObject_Collection.h"
#include "Define/GBDefine.h"

void UGBListDataObject_Collection::AddChildListData(UGBListDataObject_Base* InChildListData)
{
    GB_NULL_CHECK(InChildListData);

    if (ChildListDatas.Contains(InChildListData) == false)
    {
        InChildListData->InitDataObject();
        InChildListData->SetParentData(this);
        ChildListDatas.Add(InChildListData);
    }
}

TArray<UGBListDataObject_Base*> UGBListDataObject_Collection::GetAllChildListData() const
{
    return ChildListDatas;
}

bool UGBListDataObject_Collection::HasAnyChildListData() const
{
    return ChildListDatas.IsEmpty() == false;
}
