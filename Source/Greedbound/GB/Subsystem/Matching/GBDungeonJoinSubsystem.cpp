// Fill out your copyright notice in the Description page of Project Settings.


#include "GB/Subsystem/Matching/GBDungeonJoinSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/NetDriver.h"
#include "TimerManager.h"
#include "Misc/CoreDelegates.h"
#include "GenericPlatform/GenericPlatformHttp.h" // UrlEncode
#include "Async/Async.h"
#include "Subsystem/Lobby/GBLobbyUserManager.h"
#include "Define/GBDefine.h"

 
void UGBDungeonJoinSubsystem::Deinitialize()
{
    Super::Deinitialize();

    UnbindFailureDelegates();

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(JoinTimeoutHandle);
    }

    State = EDungeonJoinState::Idle;
}

void UGBDungeonJoinSubsystem::ConnectToDungeon(const FDungeonJoinInfo& Info, float TimeoutSec)
{
    // 소켓 스레드에서 호출될 수도 있으니, 게임 스레드 보장
    if (!IsInGameThread())
    {
        AsyncTask(ENamedThreads::GameThread, [this, Info, TimeoutSec]()
            {
                this->ConnectToDungeon(Info, TimeoutSec);
            });
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        FinishFailure(TEXT("NO_WORLD"));
        return;
    }

    // 이미 Join 중이면 중복 요청 무시(원하면 CancelJoin 호출 후 재시작으로 바꿔도 됨)
    if (IsJoining())
    {
        GB_BACKEND_LOG(Warning, TEXT("[DungeonJoin] Already joining; ignoring duplicate request."));
        return;
    }

    LastInfo = Info;
    State = EDungeonJoinState::Joining;

    // 타임아웃 설정
    World->GetTimerManager().ClearTimer(JoinTimeoutHandle);
    World->GetTimerManager().SetTimer(
        JoinTimeoutHandle,
        this, &UGBDungeonJoinSubsystem::OnJoinTimeout,
        FMath::Max(1.f, TimeoutSec),
        /*bLoop*/ false
    );

    // 실패/성공 감지 델리게이트 바인딩
    BindFailureDelegates();

    // 시작 알림
    GB_BACKEND_LOG(Warning, TEXT("GBDungeonJoin Subsystem -> Trying To Broadcast OnJoinStarted!"));

    OnJoinStarted.Broadcast(Info);

    // Refactoring 필요함.. 해킹?에 취약. 원래대로라면 서버에서 알려줘야함.
    int32 ClassInt = 0;
    if (UGameInstance* GI = GetGameInstance())
    {
        if (const UGBLobbyUserManager* LobbyUserManager = GI->GetSubsystem<UGBLobbyUserManager>())
        {
            ClassInt = static_cast<int32>(LobbyUserManager->GetMyUserInfo()->ClassType);
        }
    }

    // Travel
    FString URL = BuildTravelURL(Info);
    
    // Refactoring 필요한 부분..
    URL += FString::Printf(TEXT("?class=%d"), ClassInt);

    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC)
    {
        FinishFailure(TEXT("NO_PLAYER_CONTROLLER"));
        return;
    }

    GB_BACKEND_LOG(Log, TEXT("[DungeonJoin] ClientTravel -> %s"), *URL);
    PC->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
}

bool UGBDungeonJoinSubsystem::CancelJoin(const FString& Reason)
{
    if (!IsJoining())
    {
        return false;
    }

    FinishFailure(Reason, /*Code*/ 0);
    return true;
}

FString UGBDungeonJoinSubsystem::BuildTravelURL(const FDungeonJoinInfo& Info) const
{
    // 토큰/ID는 URL 인코딩(안전)
    const FString EncTeamId = FGenericPlatformHttp::UrlEncode(Info.TeamId);
    const FString EncDungeon = FGenericPlatformHttp::UrlEncode(Info.DungeonId);
    const FString EncMatch = FGenericPlatformHttp::UrlEncode(Info.MatchId);
    const FString EncToken = FGenericPlatformHttp::UrlEncode(Info.JoinToken);

    // UE는 옵션 연결에 ? 를 연쇄 사용
    return FString::Printf(
        TEXT("%s:%d?teamId=%s?teamSize=%d?dungeonId=%s?matchId=%s?join=%s"),
        *Info.Host, Info.Port,
        *EncTeamId, Info.TeamSize,
        *EncDungeon, *EncMatch, *EncToken
    );
}

void UGBDungeonJoinSubsystem::BindFailureDelegates()
{
    if (GEngine)
    {
        if (!NetFailHandle.IsValid())
        {
            NetFailHandle = GEngine->OnNetworkFailure().AddUObject(
                this, &UGBDungeonJoinSubsystem::HandleNetworkFailure);
        }
        if (!TravelFailHandle.IsValid())
        {
            TravelFailHandle = GEngine->OnTravelFailure().AddUObject(
                this, &UGBDungeonJoinSubsystem::HandleTravelFailure);
        }
    }

    if (!PostLoadHandle.IsValid())
    {
        PostLoadHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(
            this, &UGBDungeonJoinSubsystem::HandlePostLoadMap);
    }
}

void UGBDungeonJoinSubsystem::HandlePostLoadMap(UWorld* LoadedWorld)
{
    if (!IsJoining())
    {
        return;
    }
}

void UGBDungeonJoinSubsystem::HandleNetworkFailure(
    UWorld* World,
    UNetDriver* NetDriver,
    ENetworkFailure::Type FailureType,
    const FString& ErrorString)
{
    if (!IsJoining())
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[DungeonJoin] Network failure %d: %s"), (int32)FailureType, *ErrorString);
    const FString Reason = FString::Printf(TEXT("NETWORK_FAILURE_%d:%s"), (int32)FailureType, *ErrorString);
    FinishFailure(Reason, /*Code*/ (int32)FailureType);
}

void UGBDungeonJoinSubsystem::UnbindFailureDelegates()
{
    if (GEngine)
    {
        if (NetFailHandle.IsValid())
        {
            GEngine->OnNetworkFailure().Remove(NetFailHandle);
            NetFailHandle.Reset();
        }
        if (TravelFailHandle.IsValid())
        {
            GEngine->OnTravelFailure().Remove(TravelFailHandle);
            TravelFailHandle.Reset();
        }
    }

    if (PostLoadHandle.IsValid())
    {
        FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadHandle);
        PostLoadHandle.Reset();
    }
}

void UGBDungeonJoinSubsystem::OnJoinTimeout()
{
    if (!IsJoining())
    {
        return;
    }

    GB_BACKEND_LOG(Warning, TEXT("[DungeonJoin] Join timeout."));
    FinishFailure(TEXT("JOIN_TIMEOUT"), /*Code*/ 408);
}

void UGBDungeonJoinSubsystem::HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString)
{
    if (!IsJoining())
    {
        return;
    }

    GB_BACKEND_LOG(Warning, TEXT("[DungeonJoin] Travel failure %d: %s"), (int32)FailureType, *ErrorString);
    const FString Reason = FString::Printf(TEXT("TRAVEL_FAILURE_%d:%s"), (int32)FailureType, *ErrorString);
    FinishFailure(Reason, /*Code*/ (int32)FailureType);
}

void UGBDungeonJoinSubsystem::FinishSuccess()
{
    UnbindFailureDelegates();

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(JoinTimeoutHandle);
    }

    State = EDungeonJoinState::Idle;

    // 성공 알림 (MatchId 전달)
    OnJoinSucceeded.Broadcast(LastInfo.MatchId);
}

void UGBDungeonJoinSubsystem::FinishFailure(const FString& Reason, int32 Code)
{
    UnbindFailureDelegates();

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(JoinTimeoutHandle);
    }

    State = EDungeonJoinState::Idle;

    // 실패 알림
    OnJoinFailed.Broadcast(Reason, Code);
}
