#pragma once

#include "CoreMinimal.h"
#include "GBLobbyMatchStartButtonState.generated.h"

// 클릭 알림
DECLARE_MULTICAST_DELEGATE(FOnMatchStartClicked);

UENUM(BlueprintType)
enum class EGBLobbyMatchStartButtonState : uint8
{
    None,
};
