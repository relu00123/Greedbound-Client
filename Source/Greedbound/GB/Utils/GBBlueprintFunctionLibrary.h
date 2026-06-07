// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GBBlueprintFunctionLibrary.generated.h"

/**
 * C++ 클래스 헬퍼 전용 BP 어댑터
 */
UCLASS()
class GREEDBOUND_API UGBBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "AnimSection | Helper")
    static FName GetAttackSectionNameByComboIndex(const uint8 ComboIndex);
    
    UFUNCTION(BlueprintCallable, Category = "AnimSection | Helper")
    static FName GetDeathSectionNameByDirection(const float Direction);

    UFUNCTION(BlueprintCallable, Category = "AnimSection | Helper")
    static FName GetHitReactionNameByDirection(const float Direction);
};
