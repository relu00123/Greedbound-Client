#pragma once
#include "CoreMinimal.h"
#include "GBDungeonJoinTypes.generated.h"

 
/** 백엔드의 JoinDungeon 페이로드를 UE에서 쓰기 쉽게 모은 구조체 */
USTRUCT(BlueprintType)
struct FDungeonJoinInfo
{
    GENERATED_BODY()

    /** DS 외부 접속 주소(IP/도메인) */
    UPROPERTY(BlueprintReadOnly) FString Host;

    /** DS 포트 */
    UPROPERTY(BlueprintReadOnly) int32   Port = 7777;

    /** 던전 인스턴스/매치 식별자 (디버그/검증용) */
    UPROPERTY(BlueprintReadOnly) FString DungeonId;
    UPROPERTY(BlueprintReadOnly) FString MatchId;

    /** 팀 정보 */
    UPROPERTY(BlueprintReadOnly) FString TeamId;   // 문자열 ID
    UPROPERTY(BlueprintReadOnly) int32   TeamSize = 3;

    /** 접속 토큰(JWT 등) */
    UPROPERTY(BlueprintReadOnly) FString JoinToken;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FDungeonJoinStarted, const FDungeonJoinInfo& /*Info*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FDungeonJoinSucceeded, const FString& /*MatchId*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FDungeonJoinFailed, const FString& /*Reason*/, int32 /*Code*/);

