#pragma once
#include "CoreMinimal.h"
#include "Define/Map/GBGameMapType.h"
#include "Engine/DataAsset.h"
#include "GBGameMapCatalogData.generated.h"

UCLASS(BlueprintType)
class UGBGameMapCatalogData : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TMap<EGBGameMapType, FGBGameMapInfo>    Maps;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EGBGameMapType                          DefaultMapType = EGBGameMapType::NetTestDungeon;
};
