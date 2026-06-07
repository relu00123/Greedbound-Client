// Fill out your copyright notice in the Description page of Project Settings.

#include "GBListDataObject_Value.h"

void UGBListDataObject_Value::SetDataDynamicGetter(const TSharedPtr<FGBOptionsDataInteractionHelper>& InDynamicGetter)
{
    DataDynamicGetter = InDynamicGetter;
}

void UGBListDataObject_Value::SetDataDynamicSetter(const TSharedPtr<FGBOptionsDataInteractionHelper>& InDynamicSetter)
{
    DataDynamicSetter = InDynamicSetter;
}

FString UGBListDataObject_Value::GetDefaultValueAsString() const
{
    return DefaultStringValuep.GetValue();
}
