#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Define/Map/GBWorldTypes.h"
#include "GBWorldCatalogData.generated.h"

UCLASS(BlueprintType)
class GREEDBOUND_API UGBWorldCatalogData : public UDataAsset
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditAnywhere) 
    TArray<FGBWorldEntry> Worlds;
};
