// Fill out your copyright notice in the Description page of Project Settings.

#include "GBUIFunctionLibrary.h"
#include "ProjectSetting/GBUIDeveloperSettings.h"

TSoftClassPtr<UGBActivatableBaseWidget> UGBUIFunctionLibrary::GetSoftWidgetClassByTag(UPARAM(meta = (Categories = "UI.Widget")) FGameplayTag InWidgetTag)
{
    const UGBUIDeveloperSettings* UIDeveloperSettings = GetDefault<UGBUIDeveloperSettings>();

    checkf(UIDeveloperSettings->WidgetMap.Contains(InWidgetTag), TEXT("Could not find the corresponding widget under the tag %s"), *InWidgetTag.ToString());

    return UIDeveloperSettings->WidgetMap.FindRef(InWidgetTag);
}

TSoftObjectPtr<UTexture2D> UGBUIFunctionLibrary::GetOptionsSoftImageByTag(UPARAM(meta = (Categories = "UI.Image"))FGameplayTag ImageTag)
{
     const UGBUIDeveloperSettings* UIDeveloperSettings = GetDefault<UGBUIDeveloperSettings>();

     checkf(UIDeveloperSettings->OptionsScreenSoftImageMap.Contains(ImageTag), TEXT("Could not find the corresponding UI Image under the tag %s"), *ImageTag.ToString());

     return UIDeveloperSettings->OptionsScreenSoftImageMap.FindRef(ImageTag);
}
