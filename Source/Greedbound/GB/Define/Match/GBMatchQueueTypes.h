#pragma once
#include "CoreMinimal.h"
#include "GBMatchQueueTypes.generated.h"   // UHT 규칙: 파일명.generated.h

/** 로비 기본값: 다른 사람과 팀 허용 여부 */
UENUM(BlueprintType)
enum class EJoinPolicy : uint8
{
    Open   UMETA(DisplayName = "Open"),
    Closed UMETA(DisplayName = "Closed"),
};

namespace GBMatch
{
    // 문자열 변환(서버 JSON/로그용)
    FORCEINLINE const TCHAR* ToString(EJoinPolicy P)
    {
        return (P == EJoinPolicy::Open) ? TEXT("Open") : TEXT("Closed");
    }

    FORCEINLINE EJoinPolicy ParsePolicy(const FString& S)
    {
        return S.Equals(TEXT("Open"), ESearchCase::IgnoreCase) ? EJoinPolicy::Open
            : S.Equals(TEXT("Closed"), ESearchCase::IgnoreCase) ? EJoinPolicy::Closed
            : EJoinPolicy::Open; // 디폴트
    }
}

UENUM(BlueprintType)
enum class EGBLobbyMatchQueueState : uint8
{
    None,
    PartyHostIdle,
    PartyHostSearching,
    PartyMemberIdle,
    PartyMemberSearching,
    SoloIdle,
    SoloSearching,
    MatchAssigned,
    Pending,
    END
};

/** 현재 매칭 큐 상태 스냅샷 */
USTRUCT(BlueprintType)
struct FClientQueueState
{
    GENERATED_BODY()

    // ── 데이터(네트워킹/저장 친화) ─────────────────────────────
    UPROPERTY(BlueprintReadOnly, Category = "MatchQueue") 
    int32       MapId = 0;
    UPROPERTY(BlueprintReadOnly, Category = "MatchQueue") 
    EJoinPolicy Policy = EJoinPolicy::Open;  
    UPROPERTY(BlueprintReadOnly, Category = "MatchQueue") 
    FString     TicketId;
    UPROPERTY(BlueprintReadOnly, Category = "MatchQueue") 
    int32       PartyId = 0;  // 0 = 솔로
    UPROPERTY(BlueprintReadOnly, Category = "MatchQueue") 
    FString     LastReason;

    // ── C++ 전용 편의 함수(블루프린트에는 노출되지 않음) ─────
    bool IsInQueue()    const { return !TicketId.IsEmpty(); }
    bool IsPartyQueue() const { return PartyId > 0; }
    bool IsSoloQueue()  const { return PartyId == 0; }
    int32 GetMapID() const { return PartyId == -1 ? -1 : MapId; }

    void Reset()
    {
        MapId = 0; Policy = EJoinPolicy::Open; TicketId.Reset(); PartyId = -1; LastReason.Reset();
    }
    void SetSolo(int32 InMapId, EJoinPolicy InPolicy, const FString& InTicket)
    {
        MapId = InMapId; Policy = InPolicy; TicketId = InTicket; PartyId = 0; LastReason.Reset();
    }
    void SetParty(int32 InMapId, int32 InPartyId, EJoinPolicy InPolicy, const FString& InTicket)
    {
        MapId = InMapId; Policy = InPolicy; TicketId = InTicket; PartyId = InPartyId; LastReason.Reset();
    }
    void MarkCanceled()
    {
        //LastReason = Reason;
        TicketId.Reset(); MapId = 0; PartyId = 0; Policy = EJoinPolicy::Open;
    }

    FString ToString() const
    {
        return FString::Printf(TEXT("Queue[Ticket=%s, Map=%d, Policy=%s, Party=%d]"),
            *TicketId, MapId, GBMatch::ToString(Policy), PartyId);
    }
};
