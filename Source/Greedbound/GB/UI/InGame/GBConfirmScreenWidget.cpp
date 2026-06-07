// Fill out your copyright notice in the Description page of Project Settings.

#include "GBConfirmScreenWidget.h"
#include "Define/GBDefine.h"
#include "CommonTextBlock.h"
#include "Components/DynamicEntryBox.h"
#include "Component/GBCommonButtonBase.h"

UGBConfirmScreenInfoObject* UGBConfirmScreenInfoObject::CreateOkScreenInfoObject(const FText& InScreenTitle, const FText& InScreenMessage)
{
    UGBConfirmScreenInfoObject* InfoObject = NewObject<UGBConfirmScreenInfoObject>();
    InfoObject->ScreenTitle = InScreenTitle;
    InfoObject->ScreenMessage = InScreenMessage;

    FGBConfirmScreenButtonInfo OkButtonInfo;
    OkButtonInfo.ButtonType = EGBConfirmScreenButtonType::Closed;
    OkButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("Ok"));

    InfoObject->AvaliableScreenButtons.Add(OkButtonInfo);

    return InfoObject;
}

UGBConfirmScreenInfoObject* UGBConfirmScreenInfoObject::CreateYesNoScreenInfoObject(const FText& InScreenTitle, const FText& InScreenMessage)
{
    UGBConfirmScreenInfoObject* InfoObject = NewObject<UGBConfirmScreenInfoObject>();
    InfoObject->ScreenTitle = InScreenTitle;
    InfoObject->ScreenMessage = InScreenMessage;

    FGBConfirmScreenButtonInfo YesButtonInfo;
    YesButtonInfo.ButtonType = EGBConfirmScreenButtonType::Confirmed;
    YesButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("Yes"));

    FGBConfirmScreenButtonInfo NoButtonInfo;
    NoButtonInfo.ButtonType = EGBConfirmScreenButtonType::Cancelled;
    NoButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("No"));

    InfoObject->AvaliableScreenButtons.Add(YesButtonInfo);
    InfoObject->AvaliableScreenButtons.Add(NoButtonInfo);

    return InfoObject;
}

UGBConfirmScreenInfoObject* UGBConfirmScreenInfoObject::CreateOkCancelScreenInfoObject(const FText& InScreenTitle, const FText& InScreenMessage)
{
    UGBConfirmScreenInfoObject* InfoObject = NewObject<UGBConfirmScreenInfoObject>();
    InfoObject->ScreenTitle = InScreenTitle;
    InfoObject->ScreenMessage = InScreenMessage;

    FGBConfirmScreenButtonInfo OkButtonInfo;
    OkButtonInfo.ButtonType = EGBConfirmScreenButtonType::Confirmed;
    OkButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("Ok"));

    FGBConfirmScreenButtonInfo CancelButtonInfo;
    CancelButtonInfo.ButtonType = EGBConfirmScreenButtonType::Closed;
    CancelButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("Cancel"));

    InfoObject->AvaliableScreenButtons.Add(OkButtonInfo);
    InfoObject->AvaliableScreenButtons.Add(CancelButtonInfo);

    return InfoObject;
}

void UGBConfirmScreenWidget::InitConfirmScreen(UGBConfirmScreenInfoObject* InScreenInfoObject, TFunction<void(EGBConfirmScreenButtonType)> ClickedButtonCallback)
{
    GB_NULL_CHECK(InScreenInfoObject);
    GB_NULL_CHECK(CTB_Title);
    GB_NULL_CHECK(CTB_Message);

    CTB_Title->SetText(InScreenInfoObject->ScreenTitle);
    CTB_Message->SetText(InScreenInfoObject->ScreenMessage);

    if (DEB_Buttons->GetNumEntries() != 0)
    {
        DEB_Buttons->Reset<UGBCommonButtonBase>(
            [](UGBCommonButtonBase& ExistingButton)
            {
                ExistingButton.OnClicked().Clear();
            }
        );
    }

    GB_CONDITION_CHECK(InScreenInfoObject->AvaliableScreenButtons.IsEmpty() == false);

    for (FGBConfirmScreenButtonInfo& AvailiableButtonInfo : InScreenInfoObject->AvaliableScreenButtons)
    {
        UGBCommonButtonBase* AddedButton = DEB_Buttons->CreateEntry<UGBCommonButtonBase>();
        AddedButton->SetButtonText(AvailiableButtonInfo.ButtonTextToDisplay);
        AddedButton->OnClicked().AddLambda(
            [ClickedButtonCallback, AvailiableButtonInfo, this]()
            {
                if (ClickedButtonCallback)
                {
                    ClickedButtonCallback(AvailiableButtonInfo.ButtonType);
                    DeactivateWidget();
                }
            }
        );
    }
}

UWidget* UGBConfirmScreenWidget::NativeGetDesiredFocusTarget() const
{
    if (DEB_Buttons->GetNumEntries() != 0)
    {
        DEB_Buttons->GetAllEntries().Last()->SetFocus();
    }

    return Super::NativeGetDesiredFocusTarget();
}
