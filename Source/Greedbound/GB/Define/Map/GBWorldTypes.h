#pragma once
#include "CoreMinimal.h"
#include "Engine/World.h"
#include "GBWorldTypes.generated.h"


UENUM(BlueprintType)
enum class EGBWorldCategory : uint8 
{ 
    None, 
    Lobby, 
    Dungeon, 
    Test 
};

USTRUCT(BlueprintType)
struct GREEDBOUND_API FGBWorldEntry
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere) 
    TSoftObjectPtr<UWorld>  World;

    UPROPERTY(EditAnywhere) 
    EGBWorldCategory        WorldCategory = EGBWorldCategory::None;
};
