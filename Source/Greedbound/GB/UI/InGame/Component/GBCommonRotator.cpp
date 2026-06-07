// Fill out your copyright notice in the Description page of Project Settings.

#include "GBCommonRotator.h"
#include "CommonTextBlock.h"

void UGBCommonRotator::SetSelectedOptionByText(const FText& InOptionText)
{
    const int32 FoundIndex = TextLabels.IndexOfByPredicate(
        [InOptionText](const FText& TextItem)->bool {
            return TextItem.EqualTo(InOptionText);
        }
    );

    if (FoundIndex != INDEX_NONE)
    {
        SetSelectedItem(FoundIndex);
    }
    else
    {
        MyText->SetText(InOptionText);
    }
}
