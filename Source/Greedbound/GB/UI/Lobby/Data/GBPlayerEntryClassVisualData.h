// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Define/Character/GBClassType.h"
#include "GBPlayerEntryClassVisualData.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct FGBClassVisualData
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText                   DisplayName;
    
    UPROPERTY(EditAnywhere,BlueprintReadOnly) 
    TObjectPtr<UTexture2D>  Portrait;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UTexture2D>  Icon;
};

UCLASS()
class GREEDBOUND_API UGBPlayerEntryClassVisualData : public UDataAsset
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TMap<EGBCharacterClassType, FGBClassVisualData> ClassVisuals;
};
