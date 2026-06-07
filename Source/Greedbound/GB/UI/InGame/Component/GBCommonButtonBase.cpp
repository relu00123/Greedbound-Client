// Fill out your copyright notice in the Description page of Project Settings.

#include "GBCommonButtonBase.h"
#include "CommonTextBlock.h"
#include "CommonLazyImage.h"
#include "Input/CommonUIInputTypes.h"
#include "Subsystem/UI/GBUISubsystem.h"

void UGBCommonButtonBase::SetButtonText(const FText& InButtonText)
{
    if (CTB_ButtonText && InButtonText.IsEmpty() == false)
    {
        CTB_ButtonText->SetText(bUseUpperCaseForButtonText ? InButtonText.ToUpper() : InButtonText);
    }
}

FText UGBCommonButtonBase::GetButtonDisplayText() const
{
    return CTB_ButtonText == nullptr ? FText::GetEmpty() : CTB_ButtonText->GetText();
}

void UGBCommonButtonBase::SetButtonDisplayImage(const FSlateBrush& InBrush)
{
    if (CLI_ButtonImage)
    {
        CLI_ButtonImage->SetBrush(InBrush);
    }
}

void UGBCommonButtonBase::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (ItemAction.IsNull() == false)
    {
        ItemActionHandle = RegisterUIActionBinding(
            FBindUIActionArgs(
                ItemAction,
                true,
                FSimpleDelegate::CreateUObject(this, &UGBCommonButtonBase::OnItemActionTrigger)
            )
        );
    }
}

void UGBCommonButtonBase::NativePreConstruct()
{
    Super::NativePreConstruct();

    SetButtonText(ButtonDisplayText);
}

void UGBCommonButtonBase::NativeOnCurrentTextStyleChanged()
{
    Super::NativeOnCurrentTextStyleChanged();

    if (CTB_ButtonText && GetCurrentTextStyleClass())
    {
        CTB_ButtonText->SetStyle(GetCurrentTextStyleClass());
    }
}

void UGBCommonButtonBase::NativeOnHovered()
{
    Super::NativeOnHovered();

    if (ButtonDiscriptionText.IsEmpty() == false)
    {
        UGBUISubsystem::Get(this)->OnButtonDescriptionTextUpdated.Broadcast(this, ButtonDiscriptionText);
    }
}

void UGBCommonButtonBase::NativeOnUnhovered()
{
    Super::NativeOnUnhovered();

    if (ButtonDiscriptionText.IsEmpty() == false)
    {
        UGBUISubsystem::Get(this)->OnButtonDescriptionTextUpdated.Broadcast(this, FText::GetEmpty());
    }
}

void UGBCommonButtonBase::OnItemActionTrigger()
{
    if (OnItemAction.IsBound())
    {
        OnItemAction.Broadcast(this);
    }
}
