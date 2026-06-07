// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "GBUIDeveloperSettings.generated.h"

class UTexture2D;
class UGBActivatableBaseWidget;

UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "UI Settings"))
class GREEDBOUND_API UGBUIDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
    UPROPERTY(Config, EditAnywhere, Category = "Widget Reference", meta = (ForceInlineRow, Categories = "UI.Widget"))
    TMap<FGameplayTag, TSoftClassPtr<UGBActivatableBaseWidget>> WidgetMap;

    UPROPERTY(Config, EditAnywhere, Category = "Widget Reference", meta = (ForceInlineRow, Categories = "UI.Image"))
    TMap<FGameplayTag, TSoftObjectPtr<UTexture2D>>              OptionsScreenSoftImageMap;
};
