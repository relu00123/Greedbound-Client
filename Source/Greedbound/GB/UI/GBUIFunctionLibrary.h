// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "GBUIFunctionLibrary.generated.h"

class UGBActivatableBaseWidget;
class UTexture2D;

UCLASS()
class GREEDBOUND_API UGBUIFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "Frontend Function Library")
    static TSoftClassPtr<UGBActivatableBaseWidget>  GetSoftWidgetClassByTag(UPARAM(meta = (Categories = "UI.Widget")) FGameplayTag WidgetTag);
    
    UFUNCTION(BlueprintPure, Category = "Frontend Function Library")
    static TSoftObjectPtr<UTexture2D>               GetOptionsSoftImageByTag(UPARAM(meta = (Categories = "UI.Image")) FGameplayTag WidgetTag);
};
