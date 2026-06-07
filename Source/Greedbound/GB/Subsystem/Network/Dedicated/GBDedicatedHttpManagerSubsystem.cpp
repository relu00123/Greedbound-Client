// Fill out your copyright notice in the Description page of Project Settings.


#include "GB/Subsystem/Network/Dedicated/GBDedicatedHttpManagerSubsystem.h"
#include "HttpModule.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/CommandLine.h"
#include "Misc/CoreDelegates.h"
#include "Containers/Ticker.h"
#include "Engine/World.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Define/Match/GBMatchInfo.h"
#include "Game/Dungeon/GBDungeonGameMode.h"
#include "Utils/GBJsonUtils.h"
#include "Define/GBDefine.h"

DEFINE_LOG_CATEGORY_STATIC(LogGBDSReady, Log, All); // ← 추가

void UGBDedicatedHttpManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (!IsRunningDedicatedServer())  // DedicatedServer에서만 동작해야한다. 
    {
        return;
    }

    // 맵 로드 완료 콜백에 바인드 (listen 포트가 실제로 열리도록 약간 후에 POST)
    if (!PostLoadMapWithWorldHandle.IsValid())
    {
        //PostLoadMapWithWorldHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UGBDedicatedHttpManagerSubsystem::HandlePostLoadMap);
    }
}

void UGBDedicatedHttpManagerSubsystem::Deinitialize()
{
    if (PostLoadMapWithWorldHandle.IsValid())
    {
        //FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapWithWorldHandle);
        //PostLoadMapWithWorldHandle.Reset();
    }
    Super::Deinitialize();
}

void UGBDedicatedHttpManagerSubsystem::HandlePostLoadMap(UWorld* LoadedWorld)
{
    if (!IsRunningDedicatedServer()) // DedicatdServer에서만 동작해야한다. 
    {
        return;
    }

    GB_BACKEND_LOG(Log, TEXT("[PostLoadMap] Map=%s, WorldPort=%d"), *GetNameSafe(LoadedWorld), LoadedWorld ? LoadedWorld->URL.Port : -1);

    // 아주 조금 지연 후 dungeonReady 발신 (지연 안쓰고 있기는 함) 
    FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateWeakLambda(this, [this](float)
            {
                GB_BACKEND_LOG(Warning, TEXT("[Ticker] Fire PostDungeonReadyFromCmdline"));
                PostDungeonReady(0.f);
                return false; // 발송하면 끝. (한 번만 실행하고 제거하라) 
            }),
        0.5f
    );
}

// 실행중인 로컬 IP주소를 알아내서 문자열로 반환 (즉.. DS가 실행중인 로컬 IP주소) 
FString UGBDedicatedHttpManagerSubsystem::ResolveLocalHostIp()
{
    bool bCanBindAll = false;
    TSharedRef<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBindAll);
    return Addr->ToString(false);
}

// BS에서 받은 "DungeonReady" 응답 처리 진입점
void UGBDedicatedHttpManagerSubsystem::OnDungeonReadyResponse(bool bOK, int32 Code, TSharedPtr<FJsonObject> Json)
{
    GB_BACKEND_LOG(Warning, TEXT("[DSReady] ok=%d code=%d"), bOK ? 1 : 0, Code);

    if (!bOK || Code != 200 || !Json.IsValid())
    {
        GB_BACKEND_LOG(Warning, TEXT("[DSReady] invalid response"));
        return;
    }

    FGBMatchInfo MatchInfo;
    if (!TryParseMatchInfo(Json, MatchInfo))
    {
        FString Err; Json->TryGetStringField(TEXT("error"), Err);
        GB_BACKEND_LOG(Warning, TEXT("[DSReady] parse fail: %s"), *Err);
        return;
    }

    GB_BACKEND_LOG(Warning, TEXT("[DSReady] Match=%s Map=%d Teams=%d Expected=%d"), *MatchInfo.MatchId, MatchInfo.MapId, MatchInfo.Teams.Num(), MatchInfo.ExpectedPlayers);

    // GameMode 반영은 GameThread에서
    SetMatchInfoToGameModeOnGameThread(MatchInfo);
}

// JSON → FGBMatchInfo 파싱
bool UGBDedicatedHttpManagerSubsystem::TryParseMatchInfo(const TSharedPtr<FJsonObject>& Json, FGBMatchInfo& OutMatch) const
{
    if (!Json.IsValid())
    {
        return false;
    }

    bool bOkField = false;
    if (!Json->TryGetBoolField(TEXT("ok"), bOkField) || !bOkField)
    {
        return false;
    }

    const TSharedPtr<FJsonObject>* MatchObj = nullptr;
    if (!Json->TryGetObjectField(TEXT("match"), MatchObj) || !MatchObj) return false;

    // matchId / mapId
    (void)(*MatchObj)->TryGetStringField(TEXT("matchId"), OutMatch.MatchId);
    (void)(*MatchObj)->TryGetNumberField(TEXT("mapId"), OutMatch.MapId);

    // teams
    OutMatch.Teams.Reset();
    const TArray<TSharedPtr<FJsonValue>>* TeamsJsonPtr = nullptr;
    if ((*MatchObj)->TryGetArrayField(TEXT("teams"), TeamsJsonPtr) && TeamsJsonPtr)
    {
        OutMatch.Teams.Reserve(TeamsJsonPtr->Num());
        for (const TSharedPtr<FJsonValue>& V : *TeamsJsonPtr)
        {
            const TSharedPtr<FJsonObject>* TObj = nullptr;
            if (!V.IsValid() || !V->TryGetObject(TObj) || !TObj)
            {
                continue;
            }

            FGBTeamInfo Team;
            (void)(*TObj)->TryGetStringField(TEXT("teamId"), Team.TeamId);
            FGBJsonUtils::ReadStringArray(*TObj, TEXT("members"), Team.Members);
            FGBJsonUtils::ReadStringArray(*TObj, TEXT("sourceTickets"), Team.SourceTickets);
            OutMatch.Teams.Add(MoveTemp(Team));
        }
    }

    // expectedPlayers
    (void)Json->TryGetNumberField(TEXT("expectedPlayers"), OutMatch.ExpectedPlayers);
    return true;
}

void UGBDedicatedHttpManagerSubsystem::SetMatchInfoToGameModeOnGameThread(const FGBMatchInfo& AssignedMatchInfo)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    AsyncTask(ENamedThreads::GameThread, [World, AssignedMatchInfo]()
        {
            if (AGBDungeonGameMode* GBDungeonGameMode = World->GetAuthGameMode<AGBDungeonGameMode>())
            {
                // DungeonMode에 정보들이 잘 들어오는지 로그를 찍어보자.
                
                GB_BACKEND_LOG(Log, TEXT("=== [FGBMatchInfo Dump] ==="));
                GB_BACKEND_LOG(Log, TEXT("MatchId: %s"), *AssignedMatchInfo.MatchId);
                GB_BACKEND_LOG(Log, TEXT("MapId: %d"), AssignedMatchInfo.MapId);
                GB_BACKEND_LOG(Log, TEXT("ExpectedPlayers: %d"), AssignedMatchInfo.ExpectedPlayers);
                GB_BACKEND_LOG(Log, TEXT("Teams.Num(): %d"), AssignedMatchInfo.Teams.Num());

                for (int32 TeamIdx = 0; TeamIdx < AssignedMatchInfo.Teams.Num(); ++TeamIdx)
                {
                    const FGBTeamInfo& Team = AssignedMatchInfo.Teams[TeamIdx];
                    GB_BACKEND_LOG(Log, TEXT("  [Team %d] TeamId: %s"), TeamIdx, *Team.TeamId);

                    // Members
                    FString MembersStr;
                    for (int32 i = 0; i < Team.Members.Num(); ++i)
                    {
                        MembersStr += FString::Printf(TEXT("%s%s"), *Team.Members[i], (i < Team.Members.Num() - 1) ? TEXT(", ") : TEXT(""));
                    }
                    GB_BACKEND_LOG(Log, TEXT("     Members: %s"), *MembersStr);

                    // SourceTickets
                    FString TicketsStr;
                    for (int32 i = 0; i < Team.SourceTickets.Num(); ++i)
                    {
                        TicketsStr += FString::Printf(TEXT("%s%s"), *Team.SourceTickets[i], (i < Team.SourceTickets.Num() - 1) ? TEXT(", ") : TEXT(""));
                    }
                    GB_BACKEND_LOG(Log, TEXT("     SourceTickets: %s"), *TicketsStr);
                }

                GB_BACKEND_LOG(Log, TEXT("============================"));

                GBDungeonGameMode->SetAssignedMatchInfo(AssignedMatchInfo);
                GBDungeonGameMode->InitFromAssignedMatchInfo();
            }
        });
}

void UGBDedicatedHttpManagerSubsystem::SendDungeonReadyNow()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    GB_BACKEND_LOG(Verbose, TEXT("[Ready] CmdLine: %s"), FCommandLine::Get());

    FString ReadyUrl, ReadySecret, DungeonId, MatchId, HostStr;
    int32   PortCli = 0;

    FParse::Value(FCommandLine::Get(), TEXT("-readyUrl="), ReadyUrl);
    FParse::Value(FCommandLine::Get(), TEXT("-readySecret="), ReadySecret);
    FParse::Value(FCommandLine::Get(), TEXT("-dungeonid="), DungeonId);
    FParse::Value(FCommandLine::Get(), TEXT("-matchid="), MatchId);
    FParse::Value(FCommandLine::Get(), TEXT("-host="), HostStr);
    FParse::Value(FCommandLine::Get(), TEXT("-port="), PortCli);

    if (ReadyUrl.IsEmpty())
    {
        GB_BACKEND_LOG(Warning, TEXT("[DSReady] -readyUrl missing; skip"));
        return;
    }

    const int32 Port = (PortCli > 0) ? PortCli : (World->URL.Port > 0 ? World->URL.Port : 7777);
    if (HostStr.IsEmpty()) HostStr = ResolveLocalHostIp();

    auto Fire = [this, ReadyUrl, ReadySecret, DungeonId, MatchId, HostStr, Port]()
        {
            TSharedRef<FJsonObject> Obj = MakeShared<FJsonObject>();
            if (!DungeonId.IsEmpty())
            {
                Obj->SetStringField(TEXT("dungeonId"), DungeonId);
            }
            if (!MatchId.IsEmpty())
            {
                Obj->SetStringField(TEXT("matchId"), MatchId);
            }
            Obj->SetStringField(TEXT("host"), HostStr);
            Obj->SetNumberField(TEXT("port"), Port);

            TMap<FString, FString> Headers;
            if (!ReadySecret.IsEmpty())
            {
                // 백엔드와 정확히 맞추기: "x-ds-secret"
                Headers.Add(TEXT("x-ds-secret"), ReadySecret);
            }

            GB_BACKEND_LOG(Warning, TEXT("[DSReady] POST %s (host=%s port=%d)"), *ReadyUrl, *HostStr, Port);

            PostJson(ReadyUrl, Obj, Headers,
                [this](bool bOK, int32 Code, TSharedPtr<FJsonObject> Json)
                {
                    OnDungeonReadyResponse(bOK, Code, MoveTemp(Json));
                });
        };
        Fire();
}

/** 커맨드라인(-readyUrl, -readySecret, -dungeonid, -matchid, -host, -port)에서 읽어 dungeonReady 전송 */
void UGBDedicatedHttpManagerSubsystem::PostDungeonReady(float DelaySeconds)
{
    if (!IsRunningDedicatedServer())
    {
        return;
    }

    // 아주 조금 지연 후 dungeonReady 발신 (지연 안쓰고 있기는 함) 
    FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateWeakLambda(this, [this](float)
            {
                GB_BACKEND_LOG(Warning, TEXT("[Ticker] Fire PostDungeonReadyFromCmdline"));
                SendDungeonReadyNow();
                return false; // 발송하면 끝. (한 번만 실행하고 제거하라) 
            }),
        DelaySeconds
    );
}

// HTTP Post 요청을 보내고, JSON Body를 포함한다. 
void UGBDedicatedHttpManagerSubsystem::PostJson(const FString& Url, const TSharedRef<FJsonObject>& Json,
    const TMap<FString, FString>& Headers, TFunction<void(bool /*bOK*/, int32 /*Code*/, TSharedPtr<FJsonObject> /*Resp*/)> Completion)
{
    auto& Http = FHttpModule::Get();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Req = Http.CreateRequest();
    Req->SetURL(Url);
    Req->SetVerb(TEXT("POST"));
    Req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    for (const auto& KV : Headers)
    {
        Req->SetHeader(KV.Key, KV.Value);
    }

    FString Body;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Body);
    FJsonSerializer::Serialize(Json, Writer);
    Req->SetContentAsString(Body);

    Req->OnProcessRequestComplete().BindWeakLambda(this,
        [Completion](FHttpRequestPtr, FHttpResponsePtr Res, bool bOK)
        {
            int32 Code = Res.IsValid() ? Res->GetResponseCode() : -1;

            TSharedPtr<FJsonObject> Parsed;
            if (Res.IsValid())
            {
                const FString RespStr = Res->GetContentAsString();
                TSharedRef<TJsonReader<>> R = TJsonReaderFactory<>::Create(RespStr);
                FJsonSerializer::Deserialize(R, Parsed);
            }

            if (Completion) Completion(bOK, Code, Parsed);
        });
    Req->ProcessRequest();
}

 
