// Fill out your copyright notice in the Description page of Project Settings.

#include "GBListDataObject_StrResolution.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectSetting/GBGameUserSettings.h"
#include "UI/InGame/Options/GBOptionsDataInteractionHelper.h"

void UGBListDataObject_StrResolution::InitResolutionValues()
{
    TArray<FIntPoint> AvaliableResolutions;
    UKismetSystemLibrary::GetSupportedFullscreenResolutions(AvaliableResolutions);

    AvaliableResolutions.Sort(
        [](const FIntPoint& A, const FIntPoint& B)->bool
        {
            return A.SizeSquared() < B.SizeSquared();
        }
    );

    for (const FIntPoint& Resolution : AvaliableResolutions)
    {
        AddDynamicOption(ResToValueString(Resolution), ResToDisplayText(Resolution));
    }

    MaximumAllowedResolution = ResToValueString(AvaliableResolutions.Last());

    SetDefaultValueFromString(MaximumAllowedResolution);
}

void UGBListDataObject_StrResolution::OnDataObjectInitialized()
{
    Super::OnDataObjectInitialized();

    if (TrySetDisplayTextFromStringValue(CurrentStringValue) == false)
    {
        CurrentDisplayText = ResToDisplayText(UGBGameUserSettings::Get()->GetScreenResolution());
    }
}

bool UGBListDataObject_StrResolution::CanSetToForcedStringValue(const FString& InForcedValue) const
{
    return CurrentStringValue != InForcedValue;
}

void UGBListDataObject_StrResolution::OnSetToForcedStringValue(const FString& InForcedValue)
{
    CurrentStringValue = InForcedValue;
    TrySetDisplayTextFromStringValue(CurrentStringValue);

    if (DataDynamicSetter)
    {
        DataDynamicSetter->SetValueFromString(CurrentStringValue);

        NotifyListDataModified(this, EGBOptionsListDataModifyReason::DependencyModified);
    }
}

FString UGBListDataObject_StrResolution::ResToValueString(const FIntPoint& InResolution) const
{
    return FString::Printf(TEXT("(X=%i, Y=%i)"), InResolution.X, InResolution.Y);
}

FText UGBListDataObject_StrResolution::ResToDisplayText(const FIntPoint& InResolution) const
{
    const FString DisplayString = FString::Printf(TEXT("%i x %i"), InResolution.X, InResolution.Y);

    return FText::FromString(DisplayString);
}
