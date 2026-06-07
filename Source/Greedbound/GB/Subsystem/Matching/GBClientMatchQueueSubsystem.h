// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Define/Match/GBMatchQueueTypes.h"
#include "GBClientMatchQueueSubsystem.generated.h"

// 네이티브 델리게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnQueueJoined, const FClientQueueState& /*State*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnQueueCanceled, const FClientQueueState& /*State*/);
//DECLARE_MULTICAST_DELEGATE_OneParam(FOnMatchFound, const FClientQueueState& /*State*/);

/**
 * 
 */
UCLASS()
class GREEDBOUND_API UGBClientMatchQueueSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:
    virtual void                Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void                Deinitialize() override;

    // 현재 큐 상태 스냅샷 
    FORCEINLINE const           FClientQueueState& GetState() const { return CurrentQueueState; }

    FORCEINLINE bool            IsInQueue() const { return CurrentQueueState.IsInQueue(); }
    FORCEINLINE bool            IsPartyQueue() const { return CurrentQueueState.IsPartyQueue(); }
    FORCEINLINE bool            IsSoloQueue() const { return CurrentQueueState.IsSoloQueue(); }
    FORCEINLINE int32           GetMapID() const { return CurrentQueueState.GetMapID(); }

    /** 로비 기본 조인 정책(Get/Set). 서버로 Join 요청 보낼 때 사용(옵션) */
    FORCEINLINE EJoinPolicy     GetJoinPolicy() const { return JoinPolicy; }
    FORCEINLINE void            SetJoinPolicy(EJoinPolicy InPolicy) { JoinPolicy = InPolicy; }

    // 리셋
    void                        ResetCurrentQueueState();

    // 서버 : QueueJoined
    void                        HandleQueueJoined(int32 MapId, EJoinPolicy Policy, const FString& TicketId, int32 PartyId /*0=solo*/);

    /** 서버: QueueCanceled */
    void                        HandleQueueCanceled(int32 MapId, /*const FString& Reason,*/ int32 PartyId /*0=solo*/);

    /** 서버: MatchFound (여기선 상태만 다루고, 접속/씬전환은 다른 시스템으로 넘겨도 됨) */  
    //void                      HandleMatchFound(int32 MapId, const FString& MatchId, const FString& TeamId, const TArray<FString>& TeamMembers, const FString& ServerAddr, const FString& TokenForMe  /*내 토큰만 받는다고 가정*/);

    void                        HandleMatchAssigned(int32 MapId, const FString& MatchId, const FString& TeamId, const TArray<FString>& TeamMembers);
    void                        HandleDungeonReady(const FString& MatchId, const FString& ServerAddr, const FString& Token);
    void                        HandleMatchFailed(const FString& MatchId, const FString& Reason);

    // 클라 쪽 취소 요청(HTTP/소켓 송신은 .cpp에서)
    void                        RequestMatchQueueCancel();
    void                        RequestMatchQueueJoin();

    // ===== 바인딩/언바인딩 API (선호 패턴) =====
    FORCEINLINE FDelegateHandle BindOnQueueJoined(const FOnQueueJoined::FDelegate& Delegate) { return OnQueueJoined.Add(Delegate); }
    FORCEINLINE void            UnbindOnQueueJoined(FDelegateHandle Handle) { OnQueueJoined.Remove(Handle); }

    FORCEINLINE FDelegateHandle BindOnQueueCanceled(const FOnQueueCanceled::FDelegate& Delegate) { return OnQueueCanceled.Add(Delegate); }
    FORCEINLINE void            UnbindOnQueueCanceled(FDelegateHandle Handle) { OnQueueCanceled.Remove(Handle); }

    EGBLobbyMatchQueueState     GetLobbyMatchQueueState();

    //FORCEINLINE FDelegateHandle BindOnMatchFound(const FOnMatchFound::FDelegate& Delegate) { return OnMatchFound.Add(Delegate); }
    //FORCEINLINE void            UnbindOnMatchFound(FDelegateHandle Handle) { OnMatchFound.Remove(Handle); }

 
private:
    FClientQueueState           CurrentQueueState;                 // 현재 큐 스냅샷
    EJoinPolicy                 JoinPolicy = EJoinPolicy::Open; // 로비 기본 토글

    FOnQueueJoined              OnQueueJoined;
    FOnQueueCanceled            OnQueueCanceled;
    //FOnMatchFound             OnMatchFound;

    void                        LogState(const TCHAR* Prefix) const;

    
    
    
};
