// Fill out your copyright notice in the Description page of Project Settings.

#include "GBListEntryWidget_KeyRemap.h"
#include "Define/GBDefine.h"
#include "Define/GBTags.h"
#include "UI/InGame/Options/DataObjects/GBListDataObject_KeyRemap.h"
#include "UI/InGame/Component/GBCommonButtonBase.h"
#include "UI/InGame/Options/GBKeyRemapScreenWidget.h"
#include "UI/GBUIFunctionLibrary.h"
#include "Subsystem/UI/GBUISubsystem.h"

void UGBListEntryWidget_KeyRemap::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    CB_RemapKey->OnClicked().AddUObject(this, &UGBListEntryWidget_KeyRemap::OnRemapKeyButtonClicked);
    CB_ResetKeyBinding->OnClicked().AddUObject(this, &UGBListEntryWidget_KeyRemap::OnResetKeyBindingButtonClicked);
}

void UGBListEntryWidget_KeyRemap::OnOwingListDataObjectSet(UGBListDataObject_Base* InOwningListDataObject)
{
    GB_NULL_CHECK(InOwningListDataObject);

    Super::OnOwingListDataObjectSet(InOwningListDataObject);

    CachedOwningKeyRemapDataObject = Cast<UGBListDataObject_KeyRemap>(InOwningListDataObject);
    GB_NULL_CHECK(CachedOwningKeyRemapDataObject);

    CB_RemapKey->SetButtonDisplayImage(CachedOwningKeyRemapDataObject->GetIconFromCurrentKey());
}

void UGBListEntryWidget_KeyRemap::OnOwningListDataObjectModified(UGBListDataObject_Base* InListDataObject, EGBOptionsListDataModifyReason ModifyReason)
{
    GB_NULL_CHECK(CachedOwningKeyRemapDataObject);
    CB_RemapKey->SetButtonDisplayImage(CachedOwningKeyRemapDataObject->GetIconFromCurrentKey());
}

void UGBListEntryWidget_KeyRemap::OnRemapKeyButtonClicked()
{
    SelectThisEntryWidget();

    UGBUISubsystem::Get(this)->PushSoftWidgetToStackAynsc(GBTag::UI_WidgetStack_Modal,
        UGBUIFunctionLibrary::GetSoftWidgetClassByTag(GBTag::UI_Widget_KeyRemapScreen),
        [this](EGBAsyncPushWidgetState PushState, UGBActivatableBaseWidget* PushedWidget)
        {
            if (PushState == EGBAsyncPushWidgetState::OnCreatedBeforePush)
            {
                UGBKeyRemapScreenWidget* CreateKeyRemapScreen = CastChecked<UGBKeyRemapScreenWidget>(PushedWidget);
                CreateKeyRemapScreen->OnKeyRemapScreenKeyPressed.BindUObject(this, &UGBListEntryWidget_KeyRemap::OnKeyToRemapPressed);
                CreateKeyRemapScreen->OnKeyRemapScreenKeySelectedCanceled.BindUObject(this, &UGBListEntryWidget_KeyRemap::OnKeyRemapCanceled);

                GB_NULL_CHECK(CachedOwningKeyRemapDataObject);
                CreateKeyRemapScreen->SetDesiredInputTypeToFilter(CachedOwningKeyRemapDataObject->GetDesiredInputKeyType());
            }
        }
    );
}

void UGBListEntryWidget_KeyRemap::OnResetKeyBindingButtonClicked()
{
    SelectThisEntryWidget();

    GB_NULL_CHECK(CachedOwningKeyRemapDataObject);

    if (CachedOwningKeyRemapDataObject->CanResetBackToDefaultValue() == false)
    {
        UGBUISubsystem::Get(this)->PushConfirmScreenToModalStackAynsc(
            EGBConfirmScreenType::Ok,
            FText::FromString(TEXT("Reset Key Mapping")),
            FText::FromString(TEXT("The key binding for") + CachedOwningKeyRemapDataObject->GetDataDisplayName().ToString() + TEXT("is already set to defatul")),
            [](EGBConfirmScreenButtonType ClickedButton) {}
        );

        return;
    }

    UGBUISubsystem::Get(this)->PushConfirmScreenToModalStackAynsc(
        EGBConfirmScreenType::YesNo,
        FText::FromString(TEXT("Reset Key Mapping")),
        FText::FromString(TEXT("Are you sure you want to reset the key binding for ") + CachedOwningKeyRemapDataObject->GetDataDisplayName().ToString() + TEXT(" ?")),
        [this](EGBConfirmScreenButtonType ClickedButton) {
            if (ClickedButton == EGBConfirmScreenButtonType::Confirmed)
            {
                CachedOwningKeyRemapDataObject->TryResetBackToDefaultValue();
            }
        }
    );

}

void UGBListEntryWidget_KeyRemap::OnKeyToRemapPressed(const FKey& PressedKey)
{
    GB_NULL_CHECK(CachedOwningKeyRemapDataObject);

    CachedOwningKeyRemapDataObject->BindNewInputKey(PressedKey);
}

void UGBListEntryWidget_KeyRemap::OnKeyRemapCanceled(const FString& CanceledReason)
{
    UGBUISubsystem::Get(this)->PushConfirmScreenToModalStackAynsc(
        EGBConfirmScreenType::Ok,
        FText::FromString(TEXT("Key Remap")),
        FText::FromString(CanceledReason),
        [](EGBConfirmScreenButtonType){}
    );
}
