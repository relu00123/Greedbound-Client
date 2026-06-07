#pragma once

#include "CoreMinimal.h"
#include "Define/Character/GBClassType.h"
#include "Define/GamePhase/GBClientGamePhase.h"
#include "UserInfo.generated.h"

USTRUCT(BlueprintType)
struct FUserInfo
{
    GENERATED_BODY()

    UPROPERTY()
    FString                 NickName;

    UPROPERTY()
    EGBCharacterClassType   ClassType = EGBCharacterClassType::None;

    UPROPERTY()
    EGBClientGamePhase      ClientGamePhase = EGBClientGamePhase::None;

    UPROPERTY()
    bool                    bOnline = true;
};

USTRUCT()
struct FProfileContext
{
    GENERATED_BODY()

    UPROPERTY()
    FString TargetNickName;

    UPROPERTY()
    bool    bIsFriend = false;

    UPROPERTY()
    bool    bIsInParty = false;

    UPROPERTY()
    bool    bIsBlocked = false;
};
