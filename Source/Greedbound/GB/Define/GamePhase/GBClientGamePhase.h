#pragma once
//#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EGBClientGamePhase : uint8
{
    None,
    GameInitializing,
    Lobby,
    InMatchingQueue,
    MatchAssigned,
    JoiningDungeon,
    InDungeon,
    GameFinished,
    Pending,
};


DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMyGamePhaseChanged, EGBClientGamePhase /*From*/, EGBClientGamePhase /*To*/);
