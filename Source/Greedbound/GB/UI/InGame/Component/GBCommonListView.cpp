// Fill out your copyright notice in the Description page of Project Settings.

#include "GBCommonListView.h"
#include "Editor/WidgetCompilerLog.h"
#include "Data/UI/GBDataListEntryMappingData.h"
#include "UI/InGame/Options/ListEntries/GBListEntryWidget_Base.h"
#include "UI/InGame/Options/DataObjects/GBListDataObject_Base.h"
#include "UI/InGame/Options/DataObjects/GBListDataObject_Collection.h"
#include "Define/GBDefine.h"

UUserWidget& UGBCommonListView::OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable)
{
    if (IsDesignTime() == false)
    {
        TSubclassOf<UGBListEntryWidget_Base> FoundEntryWidgetClass = DataListEntryMapping->FindEntryWidgetClassByDataObject(Cast<UGBListDataObject_Base>(Item));
        if (FoundEntryWidgetClass)
        {
            return GenerateTypedEntry<UGBListEntryWidget_Base>(FoundEntryWidgetClass, OwnerTable);
        }
    }

    return Super::OnGenerateEntryWidgetInternal(Item, DesiredEntryClass, OwnerTable);
}

bool UGBCommonListView::OnIsSelectableOrNavigableInternal(UObject* FirstSelectedItem)
{
    GB_NULL_CHECK_WITH_RETURN(FirstSelectedItem, false);

    return FirstSelectedItem->IsA<UGBListDataObject_Collection>() == false;
}

#if WITH_EDITOR	
void UGBCommonListView::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
    Super::ValidateCompiledDefaults(CompileLog);

    if (DataListEntryMapping == nullptr)
    {
        CompileLog.Error(
            FText::FromString(
                TEXT("The Variable DatqaListEntryMapping has no valid data asset assigned") +
                GetClass()->GetName() +
                TEXT("needs valid data asset to function properly")
            )
        );
    }
}
#endif
