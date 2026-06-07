// Fill out your copyright notice in the Description page of Project Settings.

#include "GBOptionsDetailsViewWidget.h"
#include "CommonTextBlock.h"
#include "CommonLazyImage.h"
#include "CommonRichTextBlock.h"
#include "Define/GBDefine.h"
#include "DataObjects/GBListDataObject_Base.h"

void UGBOptionsDetailsViewWidget::UpdateDetailsViewInfo(UGBListDataObject_Base* InDataObject, const FString& InEntryWidgetClassName)
{
    GB_NULL_CHECK(InDataObject);

    CTB_Title->SetText(InDataObject->GetDataDisplayName());

    if (InDataObject->GetSoftDescriptionImage().IsNull() == false)
    {
        CLI_DescriptionImage->SetBrushFromLazyTexture(InDataObject->GetSoftDescriptionImage());
        CLI_DescriptionImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    }
    else
    {
        CLI_DescriptionImage->SetVisibility(ESlateVisibility::Collapsed);
    }

    CRT_Description->SetText(InDataObject->GetDataDescriptionRichText());

    const FString DynamicDetails = FString::Printf(
        TEXT("Data Object Class : <Bold>%s</>\n\nEntry Widget Class : <Bold>%s</>"),
        *InDataObject->GetClass()->GetName(),
        *InEntryWidgetClassName
    );

    CRT_DynamicDetails->SetText(FText::FromString(DynamicDetails));
    CRT_DisableReason->SetText(InDataObject->IsDataCurrentlyEditable() ? FText::GetEmpty() : InDataObject->GetDisabledRichText());
}

void UGBOptionsDetailsViewWidget::ClearDetailsViewInfo()
{
    CTB_Title->SetText(FText::GetEmpty());
    CLI_DescriptionImage->SetVisibility(ESlateVisibility::Collapsed);
    CRT_Description->SetText(FText::GetEmpty());
    CRT_DynamicDetails->SetText(FText::GetEmpty());
    CRT_DisableReason->SetText(FText::GetEmpty());
}

void UGBOptionsDetailsViewWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    ClearDetailsViewInfo();
}
