// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Define/Character/GBClassType.h"
#include "GBClassSelectData.generated.h"

USTRUCT(BlueprintType)
struct FGBClassPortraitEntry
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Portrait")
    TObjectPtr<UTexture2D>  Portrait        = nullptr;

    // 클래스별 색상 오버라이드를 사용할 경우
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portrait")
    bool                    bOverrideColors = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portrait", meta = (EditCondition = "bOverrideColors"))
    FLinearColor            NormalFrameColor        = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portrait", meta = (EditCondition = "bOverrideColors"))
    FLinearColor            SelectedFrameColor      = FLinearColor(1.f, 0.85f, 0.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portrait", meta = (EditCondition = "bOverrideColors"))
    FLinearColor            TempSelectedFrameColor  = FLinearColor(1.f, 0.85f, 1.f);
};


UCLASS()
class GREEDBOUND_API UGBClassSelectData : public UPrimaryDataAsset
{
    GENERATED_BODY()
    
public:
    // 공통 색상(오버라이드 없을시 사용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
    FLinearColor DefaultNormalFrameColor        = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
    FLinearColor DefaultSelectedFrameColor      = FLinearColor(0.01f, 0.4f, 0.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
    FLinearColor DefaultTempSelectedFrameColor  = FLinearColor(1.f, 1.f, 0.f);

    // 클래스별 초상화/옵션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entries")
    TMap<EGBCharacterClassType, FGBClassPortraitEntry> Entries;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool GetPortraitEntry(EGBCharacterClassType ClassType, FGBClassPortraitEntry& Out) const
    {
        if (const FGBClassPortraitEntry* Found = Entries.Find(ClassType))
        {
            Out = *Found;
            return true;
        }
        return false;
    }

    // 색상 가져오기 (오버라이드 처리 포함)
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetFrameColors(EGBCharacterClassType ClassType, FLinearColor& OutNormal, FLinearColor& OutSelected, FLinearColor& OutTempSelected) const
    {
        if (const FGBClassPortraitEntry* Found = Entries.Find(ClassType); Found && Found->bOverrideColors)
        {
            OutNormal = Found->NormalFrameColor;
            OutSelected = Found->SelectedFrameColor;
            OutTempSelected = Found->TempSelectedFrameColor;
        }
        else
        {
            OutNormal = DefaultNormalFrameColor;
            OutSelected = DefaultSelectedFrameColor;
            OutTempSelected = DefaultTempSelectedFrameColor;
        }
    }
};
