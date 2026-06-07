// Fill out your copyright notice in the Description page of Project Settings.


#include "GBListDataObject_KeyRemap.h"
#include "Define/GBDefine.h"
#include "CommonInputBaseTypes.h"
#include "CommonInputSubsystem.h"

void UGBListDataObject_KeyRemap::InitKeyRemapData(UEnhancedInputUserSettings* InOwningInputUserSettings, UEnhancedPlayerMappableKeyProfile* InKeyProfile, ECommonInputType InDesiredInputKeyType, const FPlayerKeyMapping& InOwingPlayerKeyMapping)
{
    GB_NULL_CHECK(InOwningInputUserSettings);
    GB_NULL_CHECK(InKeyProfile);

    CachedOwingInputUserSettings    = InOwningInputUserSettings;
    CachedOwningKeyProfile          = InKeyProfile;
    CachedDesiredInputKeyType       = InDesiredInputKeyType;
    CachedOwningMappingName         = InOwingPlayerKeyMapping.GetMappingName();
    CachedOwningMappableKeySlot     = InOwingPlayerKeyMapping.GetSlot();
}

FSlateBrush UGBListDataObject_KeyRemap::GetIconFromCurrentKey() const
{
    UCommonInputSubsystem* InputSubSystem = UCommonInputSubsystem::Get(CachedOwingInputUserSettings->GetLocalPlayer());
    GB_NULL_CHECK_WITH_RETURN(InputSubSystem, FSlateBrush());

    FSlateBrush FoundBrush;
    const bool bHasFoundBrush = UCommonInputPlatformSettings::Get()->TryGetInputBrush(
        FoundBrush,
        GetOwningKeyMapping()->GetCurrentKey(),
        CachedDesiredInputKeyType,
        InputSubSystem->GetCurrentGamepadName()
    );

    if (bHasFoundBrush == false)
    {
        GB_LOG(TEXT("Unable to find an icon for the key %s"), 
            *GetOwningKeyMapping()->GetCurrentKey().GetDisplayName().ToString()
        );
    }

    return FoundBrush;
}

void UGBListDataObject_KeyRemap::BindNewInputKey(const FKey& InNewKey)
{
    GB_NULL_CHECK(CachedOwingInputUserSettings);

    FMapPlayerKeyArgs KeyArgs;
    KeyArgs.MappingName = CachedOwningMappingName;
    KeyArgs.Slot = CachedOwningMappableKeySlot;
    KeyArgs.NewKey = InNewKey;

    FGameplayTagContainer Container;
    CachedOwingInputUserSettings->MapPlayerKey(KeyArgs, Container);
    CachedOwingInputUserSettings->SaveSettings();

    NotifyListDataModified(this);
}

bool UGBListDataObject_KeyRemap::HasDefaultValue() const
{
    FPlayerKeyMapping* OwningKey = GetOwningKeyMapping();
    GB_NULL_CHECK_WITH_RETURN(OwningKey, false);

    return GetOwningKeyMapping()->GetDefaultKey().IsValid();
}

bool UGBListDataObject_KeyRemap::CanResetBackToDefaultValue() const
{
    return HasDefaultValue() && GetOwningKeyMapping()->IsCustomized();
}

bool UGBListDataObject_KeyRemap::TryResetBackToDefaultValue()
{
    if (CanResetBackToDefaultValue())
    {
        GB_NULL_CHECK_WITH_RETURN(CachedOwingInputUserSettings, false);
        GetOwningKeyMapping()->ResetToDefault();

        CachedOwingInputUserSettings->SaveSettings();
        NotifyListDataModified(this, EGBOptionsListDataModifyReason::ResetToDefault);

        return true;
    }

    return false;
}

FPlayerKeyMapping* UGBListDataObject_KeyRemap::GetOwningKeyMapping() const
{
    GB_NULL_CHECK_WITH_RETURN(CachedOwningKeyProfile, nullptr);

    FMapPlayerKeyArgs KeyArgs;
    KeyArgs.MappingName = CachedOwningMappingName;
    KeyArgs.Slot = CachedOwningMappableKeySlot;

    return CachedOwningKeyProfile->FindKeyMapping(KeyArgs);
}
