// Fill out your copyright notice in the Description page of Project Settings.

#include "GBOptionsDataRegistry.h"
#include "Internationalization/StringTableRegistry.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Define/GBDefine.h"
#include "UI/InGame/Options/DataObjects/GBListDataObject_Collection.h"
#include "UI/InGame/Options/DataObjects/GBListDataObject_String.h"
#include "UI/InGame/Options/DataObjects/GBListDataObject_Scalar.h"
#include "UI/InGame/Options/DataObjects/GBListDataObject_StrResolution.h"
#include "UI/InGame/Options/DataObjects/GBListDataObject_KeyRemap.h"
#include "UI/InGame/Options/GBOptionsDataInteractionHelper.h"
#include "ProjectSetting/GBGameUserSettings.h"
#include "CommonInputSubsystem.h"

#define MAKE_OPTIONS_DATA_CONTROL(SetterOrGetterFuncName) \
    MakeShared<FGBOptionsDataInteractionHelper>(GET_FUNCTION_NAME_STRING_CHECKED(UGBGameUserSettings, SetterOrGetterFuncName))

#define GET_DESCRIPTION(InKey) LOCTABLE("/Game/GB/Data/UI/ST_OptionScreenDescription.ST_OptionScreenDescription", InKey)
#define GET_MENU_NAME(InKey) LOCTABLE("/Game/GB/Data/UI/ST_Menu.ST_Menu", InKey)

void UGBOptionsDataRegistry::InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer)
{
    InitGameplayCollectionTab();
    InitAudioCollectionTab();
    InitVideoCollectionTab();
    InitControlCollectionTab(InOwningLocalPlayer);
}

TArray<UGBListDataObject_Base*> UGBOptionsDataRegistry::GetListSourceItemBySelectedTabID(const FName& InSelectedTabID) const
{
    const TObjectPtr<UGBListDataObject_Collection>* FoundTabCollectionPtr = RegisteredOptionsTabCollections.FindByPredicate(
        [InSelectedTabID](UGBListDataObject_Collection* AvaliableTabCollection)->bool
        {
            return AvaliableTabCollection->GetDataID() == InSelectedTabID;
        }
    );

    GB_NULL_CHECK_WITH_RETURN(FoundTabCollectionPtr, TArray<UGBListDataObject_Base*>());

    UGBListDataObject_Collection* FoundTabCollection = *FoundTabCollectionPtr;

    const TArray<UGBListDataObject_Base*>& ChildListDataList = FoundTabCollection->GetAllChildListData();
    TArray<UGBListDataObject_Base*> AllChildListItems;

    for (UGBListDataObject_Base* ChildListData : ChildListDataList)
    {
        if (ChildListData)
        {
            AllChildListItems.Add(ChildListData);

            if (ChildListData->HasAnyChildListData())
            {
                FindChildListDataRecursively(ChildListData, AllChildListItems);
            }
        }
    }

    return AllChildListItems;
}

void UGBOptionsDataRegistry::FindChildListDataRecursively(UGBListDataObject_Base* InParentData, TArray<UGBListDataObject_Base*>& OutChildDataList) const
{
    GB_CONDITION_CHECK(InParentData != nullptr && InParentData->HasAnyChildListData());

    const TArray<UGBListDataObject_Base*>& ChildListDataList = InParentData->GetAllChildListData();
    for (UGBListDataObject_Base* SubShildListData : ChildListDataList)
    {
        if (SubShildListData)
        {
            OutChildDataList.Add(SubShildListData);

            if (SubShildListData->HasAnyChildListData())
            {
                FindChildListDataRecursively(SubShildListData, OutChildDataList);
            }
        }
    }
}

// Gameplay
void UGBOptionsDataRegistry::InitGameplayCollectionTab()
{
    UGBListDataObject_Collection* GameplayTabCollection = NewObject<UGBListDataObject_Collection>();
    GameplayTabCollection->SetDataID(FName("GameplayTabCollection"));
    GameplayTabCollection->SetDataDisplayName(GET_MENU_NAME("OptionsMenu_Gameplay"));
    
    {
        UGBListDataObject_String* LocalizationSetting = NewObject<UGBListDataObject_String>();
        LocalizationSetting->SetDataID(FName("Language"));
        LocalizationSetting->SetDataDisplayName(FText::FromString(TEXT("Language Setting")));
        LocalizationSetting->AddDynamicOption(TEXT("en"), FText::FromString(TEXT("English")));
        LocalizationSetting->AddDynamicOption(TEXT("ko"), FText::FromString(TEXT("Korean")));
        LocalizationSetting->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetCurrentLanguage));
        LocalizationSetting->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetCurrentLanguage));
        LocalizationSetting->SetShouldApplySettingsImmediately(true);
    
        GameplayTabCollection->AddChildListData(LocalizationSetting);
    }
    
    RegisteredOptionsTabCollections.Add(GameplayTabCollection);
}

void UGBOptionsDataRegistry::InitAudioCollectionTab()
{
    UGBListDataObject_Collection* AudioTabCollection = NewObject<UGBListDataObject_Collection>();
    AudioTabCollection->SetDataID(FName("AudioTabCollection"));
    AudioTabCollection->SetDataDisplayName(GET_MENU_NAME("OptionsMenu_Audio"));

    // Volume
    {
        UGBListDataObject_Collection* VolumeCategoryCollection = NewObject<UGBListDataObject_Collection>();
        VolumeCategoryCollection->SetDataID(FName("VolumeCategoryCollection"));
        VolumeCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("Volume")));

        // Overall
        {
            UGBListDataObject_Scalar* OverallVolume = NewObject<UGBListDataObject_Scalar>();
            OverallVolume->SetDataID(FName("OverallVolume"));
            OverallVolume->SetDataDisplayName(FText::FromString(TEXT("Overall Volume")));
            OverallVolume->SetDataDescriptionRichText(FText::FromString(TEXT("This is dscription for Overall Volume")));
            OverallVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));
            OverallVolume->SetOutputValueRange(TRange<float>(0.f, 2.f));
            OverallVolume->SetSliderStepSize(0.01f);
            OverallVolume->SetDefaultValueFromString(LexToString(1.f));
            OverallVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
            OverallVolume->SetNumberFormattingOptions(UGBListDataObject_Scalar::NoDecimal());
            OverallVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetOverallVolume));
            OverallVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetOverallVolume));
            OverallVolume->SetShouldApplySettingsImmediately(true);

            VolumeCategoryCollection->AddChildListData(OverallVolume);
        }

        // Music
        {
            UGBListDataObject_Scalar* MusicVolume = NewObject<UGBListDataObject_Scalar>();
            MusicVolume->SetDataID(FName("MusicVolume"));
            MusicVolume->SetDataDisplayName(FText::FromString(TEXT("Music Volume")));
            MusicVolume->SetDataDescriptionRichText(FText::FromString(TEXT("This is dscription for Music Volume")));
            MusicVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));
            MusicVolume->SetOutputValueRange(TRange<float>(0.f, 2.f));
            MusicVolume->SetSliderStepSize(0.01f);
            MusicVolume->SetDefaultValueFromString(LexToString(1.f));
            MusicVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
            MusicVolume->SetNumberFormattingOptions(UGBListDataObject_Scalar::NoDecimal());
            MusicVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetMusicVolume));
            MusicVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetMusicVolume));
            MusicVolume->SetShouldApplySettingsImmediately(true);

            VolumeCategoryCollection->AddChildListData(MusicVolume);
        }

        // SoundFX
        {
            UGBListDataObject_Scalar* SoundFXVolume = NewObject<UGBListDataObject_Scalar>();
            SoundFXVolume->SetDataID(FName("SoundFXVolume"));
            SoundFXVolume->SetDataDisplayName(FText::FromString(TEXT("Sound Effects Volume")));
            SoundFXVolume->SetDataDescriptionRichText(FText::FromString(TEXT("This is dscription for Sound Effects Volume")));
            SoundFXVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));
            SoundFXVolume->SetOutputValueRange(TRange<float>(0.f, 2.f));
            SoundFXVolume->SetSliderStepSize(0.01f);
            SoundFXVolume->SetDefaultValueFromString(LexToString(1.f));
            SoundFXVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
            SoundFXVolume->SetNumberFormattingOptions(UGBListDataObject_Scalar::NoDecimal());
            SoundFXVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetSoundFXVolume));
            SoundFXVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetSoundFXVolume));
            SoundFXVolume->SetShouldApplySettingsImmediately(true);

            VolumeCategoryCollection->AddChildListData(SoundFXVolume);
        }

        AudioTabCollection->AddChildListData(VolumeCategoryCollection);
    }
    
    // Sound
    {
        UGBListDataObject_Collection* SoundCategoryCollection = NewObject<UGBListDataObject_Collection>();
        SoundCategoryCollection->SetDataID(FName("SoundCategoryCollection"));
        SoundCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("Sound")));

        AudioTabCollection->AddChildListData(SoundCategoryCollection);

        // Allow Background Audio
        {
            UGBListDataObject_StringBool* AllowBackgroundAudio = NewObject<UGBListDataObject_StringBool>();
            AllowBackgroundAudio->SetDataID(FName("AllowBackgroundAudio"));
            AllowBackgroundAudio->SetDataDisplayName(FText::FromString(TEXT("Allow Background Audio")));
            AllowBackgroundAudio->OverrideTrueDisplayText(FText::FromString(TEXT("Enable")));
            AllowBackgroundAudio->OverrideFalseDisplayText(FText::FromString(TEXT("Disable")));
            AllowBackgroundAudio->SetFalseAsDefaultValue();
            AllowBackgroundAudio->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetAllowBackgroundAudio));
            AllowBackgroundAudio->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetAllowBackgroundAudio));
            AllowBackgroundAudio->SetShouldApplySettingsImmediately(true);

            SoundCategoryCollection->AddChildListData(AllowBackgroundAudio);
        }

        // HDRAudioMode
        {
            UGBListDataObject_StringBool* UseHDRAudioMode = NewObject<UGBListDataObject_StringBool>();
            UseHDRAudioMode->SetDataID(FName("UseHDRAudioMode"));
            UseHDRAudioMode->SetDataDisplayName(FText::FromString(TEXT("Use HDR Audio Mode")));
            UseHDRAudioMode->OverrideTrueDisplayText(FText::FromString(TEXT("Enable")));
            UseHDRAudioMode->OverrideFalseDisplayText(FText::FromString(TEXT("Disable")));
            UseHDRAudioMode->SetFalseAsDefaultValue();
            UseHDRAudioMode->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetUseHDRAudioMode));
            UseHDRAudioMode->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetUseHDRAudioMode));
            UseHDRAudioMode->SetShouldApplySettingsImmediately(true);

            SoundCategoryCollection->AddChildListData(UseHDRAudioMode);
        }
    }

    RegisteredOptionsTabCollections.Add(AudioTabCollection);
}

void UGBOptionsDataRegistry::InitVideoCollectionTab()
{
    const FText WindowModeDesc = LOCTABLE("/Game/GB/Data/UI/ST_OptionScreenDescription.ST_OptionScreenDescription", "WindowModeDescKey");
    UGBListDataObject_Collection* VideoTabCollection = NewObject<UGBListDataObject_Collection>();
    VideoTabCollection->SetDataID(FName("VideoTabCollection"));
    VideoTabCollection->SetDataDisplayName(GET_MENU_NAME("OptionsMenu_Video"));

    UGBListDataObject_StringEnum* CreatedWindowMode = nullptr;

    // Display Category
    {
        UGBListDataObject_Collection* DisplayCategoryCollection = NewObject<UGBListDataObject_Collection>();
        DisplayCategoryCollection->SetDataID(FName("DisplayCategoryCollection"));
        DisplayCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("Display")));

        FOptionsDataEditConditionDescriptor PackagedBuildOnlyCondition;
        PackagedBuildOnlyCondition.SetEditConditionFunc(
            []()->bool
            {
                const bool bIsEditor = GIsEditor || GIsPlayInEditorWorld;

                return bIsEditor == false;
            }
        );
        PackagedBuildOnlyCondition.SetDisabledRichReason(TEXT("\n\n<Disabled>This setting only be adjusted in a packaged build.</>"));

        // Window Mode
        {
            UGBListDataObject_StringEnum* WindowMode = NewObject<UGBListDataObject_StringEnum>();
            WindowMode->SetDataID(FName("WindowMode"));
            WindowMode->SetDataDisplayName(FText::FromString(TEXT("Window Mode")));
            WindowMode->SetDisabledRichText(GET_DESCRIPTION("WindowModeDescKey"));
            WindowMode->AddEnumOption(EWindowMode::Fullscreen, FText::FromString(TEXT("FullScreen Mode")));
            WindowMode->AddEnumOption(EWindowMode::WindowedFullscreen, FText::FromString(TEXT("Borderless Window")));
            WindowMode->AddEnumOption(EWindowMode::Windowed, FText::FromString(TEXT("Windowed")));
            WindowMode->SetDefaultValueFromEnumOption(EWindowMode::Windowed);
            WindowMode->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetFullscreenMode));
            WindowMode->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetFullscreenMode));
            WindowMode->SetShouldApplySettingsImmediately(true);

            WindowMode->AddEditCondition(PackagedBuildOnlyCondition);

            CreatedWindowMode = WindowMode;

            DisplayCategoryCollection->AddChildListData(WindowMode);
        }

        // Resolution
        {
            UGBListDataObject_StrResolution* ScreenResolution = NewObject<UGBListDataObject_StrResolution>();
            ScreenResolution->SetDataID(FName("ScreenResolution"));
            ScreenResolution->SetDataDisplayName(FText::FromString(TEXT("Screen Resolution")));
            ScreenResolution->SetDisabledRichText(GET_DESCRIPTION("ScreenResolutionsDescKey"));
            ScreenResolution->InitResolutionValues();
            ScreenResolution->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetScreenResolution));
            ScreenResolution->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetScreenResolution));
            ScreenResolution->SetShouldApplySettingsImmediately(true);

            ScreenResolution->AddEditCondition(PackagedBuildOnlyCondition);


            FOptionsDataEditConditionDescriptor WindowModeEditCondition;
            WindowModeEditCondition.SetEditConditionFunc(
                [CreatedWindowMode]()->bool
                {
                    return CreatedWindowMode->GetCurrentValueAsEnum<EWindowMode::Type>() != EWindowMode::WindowedFullscreen;
                }
            );
            WindowModeEditCondition.SetDisabledRichReason(TEXT("\n\n<Disabled>Screen Resolution is not adjustable when the 'Window Mode' is set to Borderless Window.The value must match with the maximum allowed resolution.</>"));
            WindowModeEditCondition.SetDisableForcedStringValue(ScreenResolution->GetMaxinumAllowedResolution());

            ScreenResolution->AddEditCondition(WindowModeEditCondition);
            ScreenResolution->AddEditDependencyData(CreatedWindowMode);

            DisplayCategoryCollection->AddChildListData(ScreenResolution);
        }

        VideoTabCollection->AddChildListData(DisplayCategoryCollection);
    }

    // Graphics Category
    {
        UGBListDataObject_Collection* GraphicsCategoryCollection = NewObject<UGBListDataObject_Collection>();
        GraphicsCategoryCollection->SetDataID(FName("GraphicsCategoryCollection"));
        GraphicsCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("Graphics")));

        // Gamma
        {
            UGBListDataObject_Scalar* DisplayGamma = NewObject<UGBListDataObject_Scalar>();
            DisplayGamma->SetDataID(FName("DisplayGamma"));
            DisplayGamma->SetDataDisplayName(FText::FromString(TEXT("Brightness")));
            DisplayGamma->SetDataDescriptionRichText(GET_DESCRIPTION("ScreenResolutionsDescKey"));
            DisplayGamma->SetDisplayValueRange(TRange<float>(0.f, 1.f));
            DisplayGamma->SetOutputValueRange(TRange<float>(1.7f, 2.7f)); // UE의 기본 밝기는 2.2
            DisplayGamma->SetSliderStepSize(0.01f);
            DisplayGamma->SetDisplayNumericType(ECommonNumericType::Percentage);
            DisplayGamma->SetNumberFormattingOptions(UGBListDataObject_Scalar::NoDecimal());
            DisplayGamma->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetCurrentDisplayGamma));
            DisplayGamma->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetCurrentDisplayGamma));
            DisplayGamma->SetDefaultValueFromString(LexToString(2.2f));

            GraphicsCategoryCollection->AddChildListData(DisplayGamma);
        }

        UGBListDataObject_StringInteger* CreatedOverallQuality = nullptr;

        // Overall Quality
        {
            UGBListDataObject_StringInteger* OverallQuality = NewObject<UGBListDataObject_StringInteger>();
            OverallQuality->SetDataID(FName("OverallQuality"));
            OverallQuality->SetDataDisplayName(FText::FromString(TEXT("Overall Quality")));
            OverallQuality->SetDataDescriptionRichText(GET_DESCRIPTION("OverallQualityDescKey"));
            OverallQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
            OverallQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
            OverallQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
            OverallQuality->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
            OverallQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
            OverallQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetOverallScalabilityLevel));
            OverallQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetOverallScalabilityLevel));
            OverallQuality->SetShouldApplySettingsImmediately(true);

            GraphicsCategoryCollection->AddChildListData(OverallQuality);

            CreatedOverallQuality = OverallQuality;
        }

        //Resolution Scale
        {
            UGBListDataObject_Scalar* ResolutionScale = NewObject<UGBListDataObject_Scalar>();
            ResolutionScale->SetDataID(FName("ResolutionScale"));
            ResolutionScale->SetDataDisplayName(FText::FromString(TEXT("3D Resolution")));
            ResolutionScale->SetDataDescriptionRichText(GET_DESCRIPTION("ResolutionScaleDescKey"));
            ResolutionScale->SetDisplayValueRange(TRange<float>(0.f, 1.f));
            ResolutionScale->SetOutputValueRange(TRange<float>(0.f, 1.f));
            ResolutionScale->SetSliderStepSize(0.01f);
            ResolutionScale->SetDisplayNumericType(ECommonNumericType::Percentage);
            ResolutionScale->SetNumberFormattingOptions(UGBListDataObject_Scalar::NoDecimal());
            ResolutionScale->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetResolutionScaleNormalized));
            ResolutionScale->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetResolutionScaleNormalized));
            ResolutionScale->SetShouldApplySettingsImmediately(true);

            ResolutionScale->AddEditDependencyData(CreatedOverallQuality);

            GraphicsCategoryCollection->AddChildListData(ResolutionScale);
        }

        //Global Illumination Quality
        {
            UGBListDataObject_StringInteger* GlobalIlluminationQuality = NewObject<UGBListDataObject_StringInteger>();
            GlobalIlluminationQuality->SetDataID(FName("GlobalIlluminationQuality"));
            GlobalIlluminationQuality->SetDataDisplayName(FText::FromString(TEXT("Global Illumination")));
            GlobalIlluminationQuality->SetDataDescriptionRichText(GET_DESCRIPTION("GlobalIlluminationQualityDescKey"));
            GlobalIlluminationQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
            GlobalIlluminationQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
            GlobalIlluminationQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
            GlobalIlluminationQuality->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
            GlobalIlluminationQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
            GlobalIlluminationQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetGlobalIlluminationQuality));
            GlobalIlluminationQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetGlobalIlluminationQuality));
            GlobalIlluminationQuality->SetShouldApplySettingsImmediately(true);

            GlobalIlluminationQuality->AddEditDependencyData(CreatedOverallQuality);
            CreatedOverallQuality->AddEditDependencyData(GlobalIlluminationQuality);

            GraphicsCategoryCollection->AddChildListData(GlobalIlluminationQuality);
        }

        //Shadow Quality
        {
            UGBListDataObject_StringInteger* ShadowQuality = NewObject<UGBListDataObject_StringInteger>();
            ShadowQuality->SetDataID(FName("ShadowQuality"));
            ShadowQuality->SetDataDisplayName(FText::FromString(TEXT("Shadow Quality")));
            ShadowQuality->SetDataDescriptionRichText(GET_DESCRIPTION("ShadowQualityDescKey"));
            ShadowQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
            ShadowQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
            ShadowQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
            ShadowQuality->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
            ShadowQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
            ShadowQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetShadowQuality));
            ShadowQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetShadowQuality));
            ShadowQuality->SetShouldApplySettingsImmediately(true);

            ShadowQuality->AddEditDependencyData(CreatedOverallQuality);
            CreatedOverallQuality->AddEditDependencyData(ShadowQuality);

            GraphicsCategoryCollection->AddChildListData(ShadowQuality);
        }

        //AntiAliasing Quality
        {
            UGBListDataObject_StringInteger* AntiAliasingQuality = NewObject<UGBListDataObject_StringInteger>();
            AntiAliasingQuality->SetDataID(FName("AntiAliasingQuality"));
            AntiAliasingQuality->SetDataDisplayName(FText::FromString(TEXT("Anti Aliasing")));
            AntiAliasingQuality->SetDataDescriptionRichText(GET_DESCRIPTION("AntiAliasingDescKey"));
            AntiAliasingQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
            AntiAliasingQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
            AntiAliasingQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
            AntiAliasingQuality->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
            AntiAliasingQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
            AntiAliasingQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetAntiAliasingQuality));
            AntiAliasingQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetAntiAliasingQuality));
            AntiAliasingQuality->SetShouldApplySettingsImmediately(true);

            AntiAliasingQuality->AddEditDependencyData(CreatedOverallQuality);
            CreatedOverallQuality->AddEditDependencyData(AntiAliasingQuality);

            GraphicsCategoryCollection->AddChildListData(AntiAliasingQuality);
        }

        //View Distance Quality
        {
            UGBListDataObject_StringInteger* ViewDistanceQuality = NewObject<UGBListDataObject_StringInteger>();
            ViewDistanceQuality->SetDataID(FName("ViewDistanceQuality"));
            ViewDistanceQuality->SetDataDisplayName(FText::FromString(TEXT("View Distance")));
            ViewDistanceQuality->SetDataDescriptionRichText(GET_DESCRIPTION("ViewDistanceDescKey"));
            ViewDistanceQuality->AddIntegerOption(0, FText::FromString(TEXT("Near")));
            ViewDistanceQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
            ViewDistanceQuality->AddIntegerOption(2, FText::FromString(TEXT("Far")));
            ViewDistanceQuality->AddIntegerOption(3, FText::FromString(TEXT("Very Far")));
            ViewDistanceQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
            ViewDistanceQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetViewDistanceQuality));
            ViewDistanceQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetViewDistanceQuality));
            ViewDistanceQuality->SetShouldApplySettingsImmediately(true);

            ViewDistanceQuality->AddEditDependencyData(CreatedOverallQuality);
            CreatedOverallQuality->AddEditDependencyData(ViewDistanceQuality);

            GraphicsCategoryCollection->AddChildListData(ViewDistanceQuality);
        }

        //Texture Quality
        {
            UGBListDataObject_StringInteger* TextureQuality = NewObject<UGBListDataObject_StringInteger>();
            TextureQuality->SetDataID(FName("TextureQuality"));
            TextureQuality->SetDataDisplayName(FText::FromString(TEXT("Texture Quality")));
            TextureQuality->SetDataDescriptionRichText(GET_DESCRIPTION("TextureQualityDescKey"));
            TextureQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
            TextureQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
            TextureQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
            TextureQuality->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
            TextureQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
            TextureQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetTextureQuality));
            TextureQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetTextureQuality));
            TextureQuality->SetShouldApplySettingsImmediately(true);

            TextureQuality->AddEditDependencyData(CreatedOverallQuality);
            CreatedOverallQuality->AddEditDependencyData(TextureQuality);

            GraphicsCategoryCollection->AddChildListData(TextureQuality);
        }

        //Visual Effects Quality
        {
            UGBListDataObject_StringInteger* VisualEffectQuality = NewObject<UGBListDataObject_StringInteger>();
            VisualEffectQuality->SetDataID(FName("VisualEffectQuality"));
            VisualEffectQuality->SetDataDisplayName(FText::FromString(TEXT("Visual Effect Quality")));
            VisualEffectQuality->SetDataDescriptionRichText(GET_DESCRIPTION("VisualEffectQualityDescKey"));
            VisualEffectQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
            VisualEffectQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
            VisualEffectQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
            VisualEffectQuality->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
            VisualEffectQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
            VisualEffectQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetVisualEffectQuality));
            VisualEffectQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetVisualEffectQuality));
            VisualEffectQuality->SetShouldApplySettingsImmediately(true);

            VisualEffectQuality->AddEditDependencyData(CreatedOverallQuality);
            CreatedOverallQuality->AddEditDependencyData(VisualEffectQuality);

            GraphicsCategoryCollection->AddChildListData(VisualEffectQuality);
        }

        //Reflection Quality
        {
            UGBListDataObject_StringInteger* ReflectionQuality = NewObject<UGBListDataObject_StringInteger>();
            ReflectionQuality->SetDataID(FName("ReflectionQuality"));
            ReflectionQuality->SetDataDisplayName(FText::FromString(TEXT("Reflection Quality")));
            ReflectionQuality->SetDataDescriptionRichText(GET_DESCRIPTION("ReflectionQualityDescKey"));
            ReflectionQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
            ReflectionQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
            ReflectionQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
            ReflectionQuality->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
            ReflectionQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
            ReflectionQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetReflectionQuality));
            ReflectionQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetReflectionQuality));
            ReflectionQuality->SetShouldApplySettingsImmediately(true);

            ReflectionQuality->AddEditDependencyData(CreatedOverallQuality);
            CreatedOverallQuality->AddEditDependencyData(ReflectionQuality);

            GraphicsCategoryCollection->AddChildListData(ReflectionQuality);
        }

        //Post Processing Quality
        {
            UGBListDataObject_StringInteger* PostProcessingQuality = NewObject<UGBListDataObject_StringInteger>();
            PostProcessingQuality->SetDataID(FName("PostProcessingQuality"));
            PostProcessingQuality->SetDataDisplayName(FText::FromString(TEXT("Post Processing Quality")));
            PostProcessingQuality->SetDataDescriptionRichText(GET_DESCRIPTION("PostProcessingQualityDescKey"));
            PostProcessingQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
            PostProcessingQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
            PostProcessingQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
            PostProcessingQuality->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
            PostProcessingQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
            PostProcessingQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetPostProcessingQuality));
            PostProcessingQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetPostProcessingQuality));
            PostProcessingQuality->SetShouldApplySettingsImmediately(true);

            PostProcessingQuality->AddEditDependencyData(CreatedOverallQuality);
            CreatedOverallQuality->AddEditDependencyData(PostProcessingQuality);

            GraphicsCategoryCollection->AddChildListData(PostProcessingQuality);
        }

        VideoTabCollection->AddChildListData(GraphicsCategoryCollection);
    }

    // Advanced Graphics Category
    {
        UGBListDataObject_Collection* AdvancedGraphicsCategoryCollection = NewObject<UGBListDataObject_Collection>();
        AdvancedGraphicsCategoryCollection->SetDataID(FName("AdvancedGraphicsCategoryCollection"));
        AdvancedGraphicsCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("Advanced")));

        // Vertical Sync
        {
            UGBListDataObject_StringBool* VerticalSync = NewObject<UGBListDataObject_StringBool>();
            VerticalSync->SetDataID(FName("VerticalSync"));
            VerticalSync->SetDataDisplayName(FText::FromString(TEXT("V-Sync")));
            VerticalSync->SetDataDescriptionRichText(GET_DESCRIPTION("VerticalSyncDescKey"));
            VerticalSync->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(IsVSyncEnabled));
            VerticalSync->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetVSyncEnabled));
            VerticalSync->SetFalseAsDefaultValue();
            VerticalSync->SetShouldApplySettingsImmediately(true);

            FOptionsDataEditConditionDescriptor FullScreenOnlyCondition;
            FullScreenOnlyCondition.SetEditConditionFunc(
                [CreatedWindowMode]()->bool
                {
                    return CreatedWindowMode->GetCurrentValueAsEnum<EWindowMode::Type>() == EWindowMode::Fullscreen;
                }
            );
            FullScreenOnlyCondition.SetDisabledRichReason(TEXT("\n\n<Disabled>This feature only works if the 'Window Mode' is set to 'Fullscreen.</>"));
            FullScreenOnlyCondition.SetDisableForcedStringValue(TEXT("false"));

            VerticalSync->AddEditCondition(FullScreenOnlyCondition);

            AdvancedGraphicsCategoryCollection->AddChildListData(VerticalSync);
        }

        // FrameRate
        {
            UGBListDataObject_String* FrameRateLimit = NewObject<UGBListDataObject_String>();
            FrameRateLimit->SetDataID(FName("FrameRateLimit"));
            FrameRateLimit->SetDataDisplayName(FText::FromString(TEXT("Frame Rate Limit")));
            FrameRateLimit->SetDataDescriptionRichText(GET_DESCRIPTION("FrameRateLimitDescKey"));
            FrameRateLimit->AddDynamicOption(LexToString(30.f),  FText::FromString(TEXT("30 FPS")));
            FrameRateLimit->AddDynamicOption(LexToString(60.f),  FText::FromString(TEXT("60 FPS")));
            FrameRateLimit->AddDynamicOption(LexToString(90.f),  FText::FromString(TEXT("90 FPS")));
            FrameRateLimit->AddDynamicOption(LexToString(120.f), FText::FromString(TEXT("120 FPS")));
            FrameRateLimit->AddDynamicOption(LexToString(0.f),   FText::FromString(TEXT("No Limit")));
            FrameRateLimit->SetDefaultValueFromString(LexToString(0.f));
            FrameRateLimit->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetFrameRateLimit));
            FrameRateLimit->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetFrameRateLimit));
            FrameRateLimit->SetShouldApplySettingsImmediately(true);

            AdvancedGraphicsCategoryCollection->AddChildListData(FrameRateLimit);
        }

        VideoTabCollection->AddChildListData(AdvancedGraphicsCategoryCollection);
    }

    RegisteredOptionsTabCollections.Add(VideoTabCollection);
}

void UGBOptionsDataRegistry::InitControlCollectionTab(ULocalPlayer* InOwningLocalPlayer)
{
    UGBListDataObject_Collection* ControlTabCollection = NewObject<UGBListDataObject_Collection>();
    ControlTabCollection->SetDataID(FName("ControlTabCollection"));
    ControlTabCollection->SetDataDisplayName(GET_MENU_NAME("OptionsMenu_Control"));

    UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = InOwningLocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    GB_NULL_CHECK(EnhancedInputSubsystem);

    UEnhancedInputUserSettings* UserSettings = EnhancedInputSubsystem->GetUserSettings();
    GB_NULL_CHECK(UserSettings);

    // Keyboard Mouse Category
    {
        UGBListDataObject_Collection* KeyboardMouseCategoryCollection = NewObject<UGBListDataObject_Collection>();
        KeyboardMouseCategoryCollection->SetDataID(FName("KeyboardMouseCategoryCollection"));
        KeyboardMouseCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("Keyboard & Mouse")));

        FOptionsDataEditConditionDescriptor KeyboardMouseOnlyCondition;
        KeyboardMouseOnlyCondition.SetEditConditionFunc(
            [InOwningLocalPlayer]()->bool
            {
                UCommonInputSubsystem* InputSubSystem = UCommonInputSubsystem::Get(InOwningLocalPlayer);
                GB_NULL_CHECK_WITH_RETURN(InputSubSystem, false);
                return InputSubSystem->GetCurrentInputType() == ECommonInputType::MouseAndKeyboard;
            }
        );
        KeyboardMouseOnlyCondition.SetDisabledRichReason(TEXT("\n\n<Disabled>This setting only be adjusted in a KeyboardMouse InputMode.</>"));

        {
            FPlayerMappableKeyQueryOptions KeyboardMouseOnly;
            KeyboardMouseOnly.KeyToMatch = EKeys::S;
            KeyboardMouseOnly.bMatchBasicKeyTypes = true;

            const TMap<FString, TObjectPtr<UEnhancedPlayerMappableKeyProfile>>& KeyboardProfiles = UserSettings->GetAllAvailableKeyProfiles();
            for (const TPair<FString, TObjectPtr<UEnhancedPlayerMappableKeyProfile>>& ProfilePair : KeyboardProfiles)
            {
                UEnhancedPlayerMappableKeyProfile* MappableProfile = ProfilePair.Value;
                GB_NULL_CHECK(MappableProfile);

                const TMap<FName, FKeyMappingRow>& PlayerMappingRows = MappableProfile->GetPlayerMappingRows();
                for (const TPair<FName, FKeyMappingRow>& MappingRowPair : PlayerMappingRows)
                {
                    for (const FPlayerKeyMapping& KeyMapping : MappingRowPair.Value.Mappings)
                    {
                        if (MappableProfile->DoesMappingPassQueryOptions(KeyMapping, KeyboardMouseOnly))
                        {
                            UGBListDataObject_KeyRemap* KeyMapDataObject = NewObject<UGBListDataObject_KeyRemap>();
                            KeyMapDataObject->SetDataID(KeyMapping.GetMappingName());
                            KeyMapDataObject->SetDataDisplayName(KeyMapping.GetDisplayName());
                            KeyMapDataObject->InitKeyRemapData(UserSettings, MappableProfile, ECommonInputType::MouseAndKeyboard, KeyMapping);

                            KeyMapDataObject->AddEditCondition(KeyboardMouseOnlyCondition);

                            KeyboardMouseCategoryCollection->AddChildListData(KeyMapDataObject);
                        }
                    }
                }
            }
        }

        ControlTabCollection->AddChildListData(KeyboardMouseCategoryCollection);
    }

    // Gamepad Category
    {

        FOptionsDataEditConditionDescriptor GamepadOnlyCondition;
        GamepadOnlyCondition.SetEditConditionFunc(
            [InOwningLocalPlayer]()->bool
            {
                UCommonInputSubsystem* InputSubSystem = UCommonInputSubsystem::Get(InOwningLocalPlayer);
                GB_NULL_CHECK_WITH_RETURN(InputSubSystem, false);
                return InputSubSystem->GetCurrentInputType() == ECommonInputType::Gamepad;
            }
        );
        GamepadOnlyCondition.SetDisabledRichReason(TEXT("\n\n<Disabled>This setting only be adjusted in a Gamepad InputMode.</>"));

        UGBListDataObject_Collection* GamepadCategoryCollection = NewObject<UGBListDataObject_Collection>();
        GamepadCategoryCollection->SetDataID(FName("GamepadCategoryCollection"));
        GamepadCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("Gamepad")));
        {
            FPlayerMappableKeyQueryOptions GamepadOnly;
            GamepadOnly.KeyToMatch = EKeys::Gamepad_FaceButton_Bottom;
            GamepadOnly.bMatchBasicKeyTypes = true;

            const TMap<FString, TObjectPtr<UEnhancedPlayerMappableKeyProfile>>& GamepadProfiles = UserSettings->GetAllAvailableKeyProfiles();
            for (const TPair<FString, TObjectPtr<UEnhancedPlayerMappableKeyProfile>>& ProfilePair : GamepadProfiles)
            {
                UEnhancedPlayerMappableKeyProfile* MappableProfile = ProfilePair.Value;
                GB_NULL_CHECK(MappableProfile);

                const TMap<FName, FKeyMappingRow>& PlayerMappingRows = MappableProfile->GetPlayerMappingRows();
                for (const TPair<FName, FKeyMappingRow>& MappingRowPair : PlayerMappingRows)
                {
                    for (const FPlayerKeyMapping& KeyMapping : MappingRowPair.Value.Mappings)
                    {
                        if (MappableProfile->DoesMappingPassQueryOptions(KeyMapping, GamepadOnly))
                        {
                            UGBListDataObject_KeyRemap* KeyMapDataObject = NewObject<UGBListDataObject_KeyRemap>();
                            KeyMapDataObject->SetDataID(KeyMapping.GetMappingName());
                            KeyMapDataObject->SetDataDisplayName(KeyMapping.GetDisplayName());
                            KeyMapDataObject->InitKeyRemapData(UserSettings, MappableProfile, ECommonInputType::Gamepad, KeyMapping);

                            KeyMapDataObject->AddEditCondition(GamepadOnlyCondition);

                            GamepadCategoryCollection->AddChildListData(KeyMapDataObject);
                        }
                    }
                }
            }
        }
        ControlTabCollection->AddChildListData(GamepadCategoryCollection);
    }
    RegisteredOptionsTabCollections.Add(ControlTabCollection);
}
