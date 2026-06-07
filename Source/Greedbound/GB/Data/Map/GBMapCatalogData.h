#pragma once
#include "CoreMinimal.h"
#include "Define/Map/GBMapType.h"
#include "Engine/DataAsset.h"
#include "GBMapCatalogData.generated.h"

UCLASS(BlueprintType)
class UGBMapCatalogData : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TMap<EGBMapType, FGBMapInfo>    Maps;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EGBMapType                      DefaultMapType = EGBMapType::NetTestDungeon;
};
