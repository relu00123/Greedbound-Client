#pragma once

#include "CoreMinimal.h"
#include "FEscapePayload.generated.h"

USTRUCT(BlueprintType)
struct FEscapePayload
{
    GENERATED_BODY()

    public:
    UPROPERTY()
    FString     DungeonToken;

    UPROPERTY()
    FString     PlayerToken;

    UPROPERTY()
    FDateTime   EscapeTime;
};
