#pragma once
#include "CoreMinimal.h"
#include "GBMatchInfo.generated.h"


class AGBPlayerController; 

USTRUCT()
struct FConnectedTeamState
{
    GENERATED_BODY()
    
    // 이 팀에 현재 DS에 로그인 완료된 인원 수
    int32 ConnectedCount = 0;

    // 어떤 PlayerController / PlayerID들이 들어와 왔는지를 추적
    TArray<TWeakObjectPtr<AGBPlayerController>> PlayerControllers;
};

USTRUCT()
struct FMatchRuntimeState
{
    GENERATED_BODY()
    
    // 팀별 현재 접속 상황
    // Key : TeamId 
    TMap<FString, FConnectedTeamState> TeamStates;

    // 전체 접속사 수
    int32 TotalConnectedPlayers = 0;

    // 매치가 열린 시간
    double MatchOpenTimeSeconds = 0.0;

    // 강제 시작까지 허용할 최대 대기 시간
    float MaxWaitBeforeForceStart = 60.0f;

    // 이미 매치가 시작을 하였는가?
    bool bMatchStarted = false; 
};



USTRUCT(BlueprintType)
struct FGBTeamInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString TeamId;

    UPROPERTY(BlueprintReadOnly)
    TArray<FString> Members;

    UPROPERTY(BlueprintReadOnly)
    TArray<FString> SourceTickets;
};

USTRUCT(BlueprintType)
struct FGBMatchInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString MatchId;

    UPROPERTY(BlueprintReadOnly)
    int32 MapId = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 ExpectedPlayers = 0;

    UPROPERTY(BlueprintReadOnly)
    TArray<FGBTeamInfo> Teams;
};
