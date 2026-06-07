// Fill out your copyright notice in the Description page of Project Settings.

#include "GBGameUserSettings.h"

UGBGameUserSettings::UGBGameUserSettings()
    : OverallVolume(1.f)
    , MusicVolume(1.f)
    , SoundFXVolume(1.f)
    , bAllowBackgroundAudio(false)
    , bUseHDRAudioMode(false)
{
}

UGBGameUserSettings* UGBGameUserSettings::Get()
{
    if (GEngine)
    {
        return CastChecked<UGBGameUserSettings>(GEngine->GetGameUserSettings());
    }

    return nullptr;
}

void UGBGameUserSettings::SetCurrentLanguage(const FString& InLanguege)
{
    CurrentLanguage = InLanguege;

    FInternationalization::Get().SetCurrentCulture(CurrentLanguage);
    FTextLocalizationManager::Get().RefreshResources();

    GConfig->SetString(TEXT("Internationalization"), TEXT("Culture"), *CurrentLanguage, GGameUserSettingsIni);
    GConfig->Flush(false, GGameUserSettingsIni);
}

void UGBGameUserSettings::SetOverallVolume(float InVolume)
{
    OverallVolume = InVolume;

    // 실제 볼륨 조절 로직
}

void UGBGameUserSettings::SetMusicVolume(float InVolume)
{
    MusicVolume = InVolume;
}

void UGBGameUserSettings::SetSoundFXVolume(float InVolume)
{
    SoundFXVolume = InVolume;
}

void UGBGameUserSettings::SetAllowBackgroundAudio(bool bIsAllowed)
{
    bAllowBackgroundAudio = bIsAllowed;
}

void UGBGameUserSettings::SetUseHDRAudioMode(bool bIsAllowed)
{
    bUseHDRAudioMode = bIsAllowed;
}

float UGBGameUserSettings::GetCurrentDisplayGamma() const
{
    if (GEngine)
    {
        return GEngine->GetDisplayGamma();
    }

    return 0.f;
}

void UGBGameUserSettings::SetCurrentDisplayGamma(float InNewGamma)
{
    if (GEngine)
    {
        GEngine->DisplayGamma = InNewGamma;
    }
}
