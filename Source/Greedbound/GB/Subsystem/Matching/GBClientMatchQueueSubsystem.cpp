// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/Matching/GBClientMatchQueueSubsystem.h"
#include "Subsystem/Social/GBPartyManager.h"
#include "Subsystem/Map/GBMapSelectionSubsystem.h"
#include "Subsystem/Network/Client/GBClientSocketRequestHelper.h"
#include "Subsystem/GamePhase/GBClientPhaseSubsystem.h"
void UGBClientMatchQueueSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UGBClientMatchQueueSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

void UGBClientMatchQueueSubsystem::ResetCurrentQueueState()
{
    CurrentQueueState.Reset();
}

void UGBClientMatchQueueSubsystem::HandleQueueJoined(int32 MapId, EJoinPolicy Policy, const FString& TicketId, int32 PartyId)
{
    if (PartyId > 0)
    {
        CurrentQueueState.SetParty(MapId, PartyId, Policy, TicketId);
    }
    else
    {
        CurrentQueueState.SetSolo(MapId, Policy, TicketId);
    }

    LogState(TEXT("QueueJoined"));
    OnQueueJoined.Broadcast(CurrentQueueState);
}

void UGBClientMatchQueueSubsystem::HandleQueueCanceled(int32 MapId, /*const FString& Reason*/ int32 PartyId)
{
    // 보수적으로 맵/파티 불일치 시에도 일단 취소 처리
    CurrentQueueState.MarkCanceled();

    LogState(TEXT("QueueCanceled"));
    OnQueueCanceled.Broadcast(CurrentQueueState);
}

void UGBClientMatchQueueSubsystem::HandleMatchAssigned(int32 MapId, const FString& MatchId, const FString& TeamId, const TArray<FString>& TeamMembers)
{
    // 대기 화면 전환 등
    UE_LOG(LogTemp, Log, TEXT("[MatchAssigned] map=%d match=%s team=%s members=%d"),
        MapId, *MatchId, *TeamId, TeamMembers.Num());
    // 여기선 State.TicketId는 이미 QueueJoined 때 세팅되어 있을 것.
    // 필요하면 상태에 MatchId/TeamId 캐시용 필드 추가해서 저장해도 됨.
}

void UGBClientMatchQueueSubsystem::HandleDungeonReady(const FString& MatchId, const FString& ServerAddr, const FString& Token)
{
    UE_LOG(LogTemp, Log, TEXT("[DungeonReady] match=%s addr=%s token=%s"), *MatchId, *ServerAddr, *Token);

    //  실제 접속 트리거 (예: Travel/NetConnect)
    // GEngine->SetClientTravel(GetWorld(), *FString::Printf(TEXT("%s?Token=%s"), *ServerAddr, *Token), ETravelType::TRAVEL_Absolute);

    // 상태 리셋/완료 표시
    CurrentQueueState.Reset();
    OnQueueCanceled.Broadcast(CurrentQueueState); // or 별도 OnMatchStarted 같은 델리게이트
}

void UGBClientMatchQueueSubsystem::HandleMatchFailed(const FString& MatchId, const FString& Reason)
{
    UE_LOG(LogTemp, Warning, TEXT("[MatchFailed] match=%s reason=%s"), *MatchId, *Reason);
    CurrentQueueState.MarkCanceled();
    OnQueueCanceled.Broadcast(CurrentQueueState);
}

void UGBClientMatchQueueSubsystem::RequestMatchQueueCancel()
{

    // 1. 방어코딩 
    if (!IsInQueue())
    {
        UE_LOG(LogTemp, Warning, TEXT("[MatchQueue] RequestCancel ignored: not in queue"));
        return;
    }

    int32 MapID = GetGameInstance()->GetSubsystem<UGBClientMatchQueueSubsystem>()->GetMapID();

    // 2. 서버에 요청 전송
    GBClientSocketRequestHelper::SendMatchCancelRequest(MapID); 

    // 소켓으로 취소 요청 보냄 (type은 서버 프로토콜에 맞춰 변경)
    /*TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetNumberField(TEXT("mapId"), State.MapId);
    Payload->SetStringField(TEXT("ticketId"), State.TicketId);
    if (State.PartyId > 0)
    {
        Payload->SetNumberField(TEXT("partyId"), State.PartyId);
    }

    if (UGameInstance* GI = GetGameInstance())
    {
        SendQueueControlMessage(GI, TEXT("CancelQueue"), Payload);
    }

    UE_LOG(LogTemp, Log, TEXT("[MatchQueue] Cancel requested: map=%d ticket=%s party=%d"),
        State.MapId, *State.TicketId, State.PartyId);*/
}

void UGBClientMatchQueueSubsystem::RequestMatchQueueJoin()
{
    // 1. 매치 큐에 등록할 수 있는 자격이 있는지 한번더 방어 코딩
    UGBPartyManager* PartyManager = GetGameInstance()->GetSubsystem<UGBPartyManager>();

    if (!PartyManager) return;

    EPartyRole PartyRole =  PartyManager->GetMyPartyRole();

    // Host나 솔로가 아니라면 Return
    if (!(PartyRole == EPartyRole::Host || PartyRole == EPartyRole::Solo)) return;

    // 현재 매칭중이라면 매칭큐에 등록할 수 없다. 
    if (CurrentQueueState.IsInQueue()) return;
        

    // 2. 현재 선택된 맵 가져오기 
    int32 MapNumericID = 0;
    if (UGBMapSelectionSubsystem* MapSelectionMgr = GetGameInstance()->GetSubsystem<UGBMapSelectionSubsystem>())
    {
        if (const FGBGameMapInfo* SelectedMapInfo = MapSelectionMgr->GetSelectedMapInfo())
        {
            // 서버에 보낼 맵에대한 ID 
            MapNumericID = SelectedMapInfo->MapNumericId;
        }
    }

    // 현재 선택된 맵이 없는 경우 
    if (MapNumericID == 0)
    {
           UE_LOG(LogTemp, Error, TEXT("[GBLobbyMainWidget.cpp] No Selected Map ID; aborting start match"));
           return;
    }

    // 3. 서버에 요청 전송 
    GBClientSocketRequestHelper::SendMatchStartRequest(MapNumericID);



    // 4. 서버에서 응답받기 Match StartButton 비활성화 (이건 UI측에서 바로 해도 괜찮겠다) 

 

    //if (UGBClientSessionSubsystem* Session = GetGameInstance()->GetSubsystem<UGBClientSessionSubsystem>())
   //{
   //    const FString Token = Session->GetAuthToken();

   //    // 1. 현재 선택된 맵 가져오기 
   //    int32 MapNumericID = 0;
   //    if (UGBMapSelectionSubsystem* MapSelectionMgr = GetGameInstance()->GetSubsystem<UGBMapSelectionSubsystem>())
   //    {
   //        if (const FGBGameMapInfo* SelectedMapInfo = MapSelectionMgr->GetSelectedMapInfo())
   //        {
   //            // 서버에 보낼 맵에대한 ID 
   //            MapNumericID = SelectedMapInfo->MapNumericId;
   //        }
   //    }

   //    // 1. 현재 선택된 맵이 없는 경우
   //    if (MapNumericID == 0)
   //    {
   //        UE_LOG(LogTemp, Error, TEXT("[GBLobbyMainWidget.cpp] No Selected Map ID; aborting start match"));
   //        return;
   //    }
   //        
   //    // 2. HTTP 전송 
   //    if (UGBClientHttpManagerSubsystem* Http = GetGameInstance()->GetSubsystem<UGBClientHttpManagerSubsystem>())
   //    {
   //        //Http->SendStartMatch(Token, MapNumericID, [this](bool bSuccess, const FString& Message)
   //        //    {
   //        //        if (bSuccess && GameStartButtonText)
   //        //        {
   //        //            GameStartButtonText->SetText(FText::FromString(TEXT("Searching...")));
   //        //            GameStartButton->SetIsEnabled(false); // 잠금도 가능
   //        //        }
   //        //        else
   //        //        {
   //        //            // 실패 시 팝업 띄우기 등
   //        //        }
   //        //    });
   //    }
   //}
}

EGBLobbyMatchQueueState UGBClientMatchQueueSubsystem::GetLobbyMatchQueueState()
{
    // 이부분 로직이 이상.. 작동은 하는데 유연성 부족... 

    UGBClientPhaseSubsystem* ClientPhaseSubsystem = GetGameInstance()->GetSubsystem<UGBClientPhaseSubsystem>();

    if (ClientPhaseSubsystem)
    {
        EGBClientGamePhase CurMyGamePhase = ClientPhaseSubsystem->GetMyGamePhase();

        if (CurMyGamePhase == EGBClientGamePhase::MatchAssigned)
            return EGBLobbyMatchQueueState::MatchAssigned;
    }
    
    UGBPartyManager* PartyManager = GetGameInstance()->GetSubsystem<UGBPartyManager>(); 

    if (PartyManager)
    {
        bool bIsPartyHost = PartyManager->IsPartyHost();

        // MatchQueue에 들어있는 경우
        if (CurrentQueueState.IsInQueue())
        {
            if (CurrentQueueState.IsPartyQueue())
            {
                if (bIsPartyHost)
                    return EGBLobbyMatchQueueState::PartyHostSearching;

                else
                    return EGBLobbyMatchQueueState::PartyMemberSearching;
            }

            else if (CurrentQueueState.IsSoloQueue())
            {
                return EGBLobbyMatchQueueState::SoloSearching;
            }
        }

        // MatchQueue에 들어있지 않은 경우 
        else
        {
            // 파티는 있음. 
            if (PartyManager->GetCurrentPartyInfo().IsValid())
            {
                if (bIsPartyHost)
                    return EGBLobbyMatchQueueState::PartyHostIdle;
                else
                    return EGBLobbyMatchQueueState::PartyMemberIdle;
            }

            // 솔로 
            else
            {
                return EGBLobbyMatchQueueState::SoloIdle;
            }
        }
    }

    return EGBLobbyMatchQueueState::None;
}

void UGBClientMatchQueueSubsystem::LogState(const TCHAR* Prefix) const
{
    const FString S = CurrentQueueState.ToString();
    UE_LOG(LogTemp, Log, TEXT("[MatchQueue][%s] %s"), Prefix, *S);
}
