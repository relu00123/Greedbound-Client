// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/Dungeon/GBDungeonGameMode.h"
#include "Game/Dungeon/GBDungeonGameState.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "WorldActors/SpawnPoint/GBCharacterSpawnPoint.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerState.h"
#include "Algo/AllOf.h"
#include "Engine/NetConnection.h"             
#include "GameFramework/PlayerStart.h"
#include "Subsystem/Network/Dedicated/GBDedicatedHttpManagerSubsystem.h"
#include "Character/Player/GBPlayerState.h"
#include "Data/Class/GBClassPresentationData.h"
#include "GameFramework/HUD.h"
#include "Subsystem/Network/Dedicated/GBDedicatedSessionSubsystem.h"
#include "Subsystem/Network/Dedicated/GBDedicatedSocketSubsystem.h"
#include "Character/Player/GBPlayerController.h"
#include "Define/GBDefine.h"
#include "Subsystem/Network/Dedicated/GBDedicatedSocketRequestHelper.h"
#include "Subsystem/Network/Dedicated/GBDedicatedHttpManagerSubsystem.h"
#include "Define/GBDefine.h"

// 순서 : 1
AGBDungeonGameMode::AGBDungeonGameMode()
{
    bDelayedStart = true;               // 서버가 StartMatch 시점 제어
    bStartPlayersAsSpectators = true;   // 시작 전 관전자 처리(원하면 false)

    // BP를 쓰지 않으면 이값으로 사용되지만, 우리는 GBDungeonGameMode를 상속받은 BP_GBDungeonGameMode를 사용한다.
    // 따라서 이 값은 의미가 없고 Override될 것임. 
    GameStateClass = AGBDungeonGameState::StaticClass();
    PlayerStateClass = AGBPlayerState::StaticClass();
    PlayerControllerClass = AGBPlayerController::StaticClass();
}

// 순서 : 2
void AGBDungeonGameMode::BeginPlay()
{
    Super::BeginPlay();

    // 맵에 있는 모든 AGBCharacterSpawnPoint 수집
    for (TActorIterator<AGBCharacterSpawnPoint> It(GetWorld()); It; ++It)
    {
        CharacterSpawnPool.Add(*It);
    }

    UGBDedicatedHttpManagerSubsystem* DedicatedHttpManager = GetGameInstance()->GetSubsystem<UGBDedicatedHttpManagerSubsystem>();

    if (DedicatedHttpManager)
    {
        DedicatedHttpManager->PostDungeonReady(0.f);
    }
}


// 순서 : 3 
// 이 함수는 FCoreUObjectDelegates::PostLoadMapWithWorld보다 늦게 호출된다. 
// 모든 액터의 BeginPlay()가 호출된 뒤 마지막에 실행된다. 
// 이론적으로는 위험하다. Http로 던전이 준비되었다는 패킷은 이미 보냈기 상태이기 때문에, 클라에서 DS에 입장이 가능하다.
// 하지만 Client가 접속하는 시점에 StartPlay()가 호출되지 않아 DungeonSession이 저장되지 않았던 상태라면 RaceCondition이 발생한 것이다. 
void AGBDungeonGameMode::StartPlay()
{
    Super::StartPlay();

    //BackEndServer와 연결 시도하기 위해 필요한 클래스들 가져오기
    UGBDedicatedSessionSubsystem* DedicatedSessionSubSystem = GetGameInstance()->GetSubsystem<UGBDedicatedSessionSubsystem>();
    UGBDedicatedSocketSubsystem* DedicatedSocketSubsystem = GetGameInstance()->GetSubsystem<UGBDedicatedSocketSubsystem>();

    if (!DedicatedSessionSubSystem || !DedicatedSocketSubsystem)
    {
        return;
    }

    // 던전이 이미 존재하는 경우 반환.
    // 이경우는 플레이어가 더 깊은 던전으로 갔을때 BakendServer에 중복하여 Scoket에 연결하는 것을 막기 위함. 
    if (DedicatedSessionSubSystem->IsDungeonExists())
    {
        return;
    }

    // DedicatedServer 실행할때 넘겨준인자에서 token과 dungeonID추출하기
    // Backend Server에서 받은 인자들중 유일하게 저장하는 것이다.
    FString DungeonToken, DungeonId;
    FParse::Value(FCommandLine::Get(), TEXT("dungeontoken="), DungeonToken);
    FParse::Value(FCommandLine::Get(), TEXT("dungeonid="), DungeonId);

    // 세션 저장
    DedicatedSessionSubSystem->StoreDungeonSession(DungeonToken, DungeonId);

    // 소캣 연결 시도
    DedicatedSocketSubsystem->ConnectSocket(DungeonToken);
}

// 순서 4 : 
// 클라이언트가 서버에 접속할 때 가장 먼저 호출되는 콜백.
// 아직 PlayerController, PlayerState가 없다.
// 오직 문자열(Options)과 연결 주소(Address)만 있다.
// 여기서 입장 허용 / 거절을 판별한다.
// 마치 방화벽 같은 GateKeeper. 여기서 PlayerState 같은 것은 사용할 수 없다. 
void AGBDungeonGameMode::PreLogin(const FString& Options, const FString& Address,
    const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
    Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

    const FString TeamId = UGameplayStatics::ParseOption(Options, TEXT("teamId"));

    if (!ErrorMessage.IsEmpty())
    {
        return;
    }

    // 1) DS가 가진 MAtchInfo에 팀이 존재하는지 확인한다
    const FGBTeamInfo* AssignedTeamInfo = nullptr;
    for (const FGBTeamInfo& FocusedTeamInfo : AssignedMatchInfo.Teams)
    {
        if (FocusedTeamInfo.TeamId == TeamId)
        {
            AssignedTeamInfo = &FocusedTeamInfo;
            break;
        }
    }

    if (AssignedTeamInfo == nullptr)
    {
        ErrorMessage = TEXT("InvalidTeam");
        return;
    }
}

// 순서 : 5 
// PreLogin() 통과 후 엔진이 PlayerController를 생성한 직후 호출
// 이때 이미 PlayerState도 생성되어서 NewPlayerController->PlayerState로 접근 가능
// Options 파라미터를 통해 URL 쿼리 (?taemId=xxx&class=1)를 다시 받을 수 있다.
// PlayerState 초기화는 여기서 하는것이 좋다. 
FString AGBDungeonGameMode::InitNewPlayer(APlayerController* NewPC, const FUniqueNetIdRepl& UniqueId,
    const FString& Options, const FString& Portal)
{
    FString Error = Super::InitNewPlayer(NewPC, UniqueId, Options, Portal);

    if (!Error.IsEmpty())
    {
        return Error;
    }

    const FString TeamId = UGameplayStatics::ParseOption(Options, TEXT("teamId"));
    int32   ClassInt = UGameplayStatics::GetIntOption(Options, TEXT("class"), 0);

    if (const UEnum* Enum = StaticEnum<EGBCharacterClassType>())
    {
        if (!Enum->IsValidEnumValue(ClassInt)) 
        { 
            ClassInt = 0;
        }
    }

    if (AGBPlayerState* PS = NewPC->GetPlayerState<AGBPlayerState>())
    {
        PS->SetSelectedClass(static_cast<EGBCharacterClassType>(ClassInt));
        PS->SetTeamId(TeamId);
    }

    TeamIdByController.Add(NewPC, TeamId);

    return Error;
}

// 순서 6 : 
// 클라이언트가 완전히 접속 완료한 뒤, 최종적으로 호출됨
// 이때 PlayerController, PlayerState, GameState 모두 유효
// 여기서 주로 하는 일은..
// 전체 접속 인원 카운트 증가, 팀별 상태 업데이트, TryStartMatch() 같은 함수로 매치 시작 조건 검사
// PostLogin()에서 조건 만족시 StartMatch()를 호출 할 수 있다. 
void AGBDungeonGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    if (!NewPlayer) return;

    // 1 팀 ID 결정 
    FString TeamId;
    if (AGBPlayerState* PS = NewPlayer->GetPlayerState<AGBPlayerState>())
    {
        TeamId = PS->GetTeamId(); // PlayerState에 저장되어 있다면 이게 제일 깔끔
    }
    else
    {
        TeamId = TeamIdByController.FindRef(NewPlayer); // fallback
    }

    if (TeamId.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("[DungeonGM] PostLogin: TeamId not found for %s"), *GetNameSafe(NewPlayer));
        return;
    }

    // 2) 접속 카운트 업데이트
    ConnectedPlayerCountByTeam.FindOrAdd(TeamId) += 1;

    if (AGBDungeonGameState* S = GS())
    {
        S->ConnectedPlayers = GetConnectedPlayerCount();
    }

    // 3) 실제 스폰 위치 할당
    const int32 TeamSize = ExpectedPlayerCountByTeam.FindRef(TeamId);
    
    // 4) HUD / 클래스 변경 바인딩 (기존 코드 유지)
    if (AGBPlayerState* PS = NewPlayer->GetPlayerState<AGBPlayerState>())
    {
        // 초기 HUD
        if (TSubclassOf<AHUD> HudClass = ChooseHUDFromPS(PS))
        {
            // 2025.12.08 테스트 (원래는 주석되어 있지 않음)
            //NewPlayer->ClientSetHUD(HudClass);
        }

        GB_LOG(TEXT("Set Assigned InGame HUDClass Called"));
        PS->SetAssignedInGameHUDClass(HUDClass);
    }
}

void AGBDungeonGameMode::InitFromAssignedMatchInfo()
{
    GB_BACKEND_LOG(Warning, TEXT("InitFromAssignedMatchInfo Called!"));

    // 이미 초기화된 적 있다면 중복 실행 방지하고 싶으면 가드 넣어도 됨
    if (!GetWorld())
    {
        GB_BACKEND_LOG(Warning, TEXT("InitFromAssignedMatchInfo Failed Due to GetWorld() Failed"));
        return;
    }

    GB_BACKEND_LOG(Warning, TEXT("World Exists"));

    // 1) CharacterSpawnPool은 BeginPlay에서 이미 모아놨다고 가정
    //    (만약 StartPlay 시점까지 아직 안 모아졌다면 여기서 다시 스캔해도 됨)

    ExpectedPlayerCountByTeam.Reset();
    ConnectedPlayerCountByTeam.Reset();
    TeamSpawnPointReservations.Reset();

    AGBDungeonGameState* DungeonGS = GetGameState<AGBDungeonGameState>();
    if (DungeonGS)
    {
        DungeonGS->ExpectedPlayers = AssignedMatchInfo.ExpectedPlayers;
        DungeonGS->ConnectedPlayers = 0;
    }

    // 2) AssignedMatchInfo.Teams 기반으로 채움
    for (const FGBTeamInfo& TeamInfo : AssignedMatchInfo.Teams)
    {
        const FString& TeamId = TeamInfo.TeamId;

        // 예상 인원수: 보통 Members.Num()가 그 팀의 실제 예정 인원
        const int32 ExpectedSize = TeamInfo.Members.Num();

        ExpectedPlayerCountByTeam.Add(TeamId, ExpectedSize);
        ConnectedPlayerCountByTeam.Add(TeamId, 0);

 
        // 3) 스폰 슬롯을 여기서 미리 예약 
        ReserveAnchorForTeam(TeamId, ExpectedSize);
    }

    GB_BACKEND_LOG(Warning, TEXT("[DungeonGM] InitFromAssignedMatchInfo done. Expected=%d teams=%d"),
        DungeonGS ? DungeonGS->ExpectedPlayers : -1,
        AssignedMatchInfo.Teams.Num());


    // 로그찍어서 디버깅 해보는 코드 
    GB_BACKEND_LOG(Warning, TEXT("==== [TeamSpawnPointReservations Dump] (%d entries) ===="), TeamSpawnPointReservations.Num());

    for (const auto& KV : TeamSpawnPointReservations)
    {
        const FString& TeamId = KV.Key;
        const FTeamSpawnPointReservation& R = KV.Value;

        FString AnchorName = R.Anchor.IsValid() ? R.Anchor->GetName() : TEXT("None");
        int32 SlotCount = R.Slots.Num();

        GB_BACKEND_LOG(Warning, TEXT("TeamId: %s | Anchor: %s | NextIndex: %d | Slots: %d"),
            *TeamId, *AnchorName, R.NextIndex, SlotCount);

        for (int32 i = 0; i < SlotCount; ++i)
        {
            const FVector Loc = R.Slots[i].GetLocation();
            const FRotator Rot = R.Slots[i].GetRotation().Rotator();

            GB_BACKEND_LOG(Warning, TEXT("   └─ Slot[%d] Loc=(%.1f, %.1f, %.1f) Rot=(%.1f, %.1f, %.1f)"),
                i, Loc.X, Loc.Y, Loc.Z, Rot.Pitch, Rot.Yaw, Rot.Roll);
        }
    }

    GB_BACKEND_LOG(Warning, TEXT("======================================================="));
}

void AGBDungeonGameMode::HandleClientLoadedDungeonMap(AGBPlayerController* PlayerController)
{
    GB_BACKEND_LOG(Warning, TEXT("HandleClinet Loaded DungeonMap Called!"));

    if (!PlayerController)
    {
        return; 
    }

    AGBPlayerState* PlayerState = PlayerController->GetPlayerState<AGBPlayerState>();
    if (PlayerState)
    {
        const FString& PlayerToken =  PlayerState->GetAuthToken();

        UGBDedicatedSocketRequestHelper::SendChangeClientGamePhase(PlayerToken, EGBClientGamePhase::InDungeon);
    }

    if (AGBDungeonGameState* DungeonGameState = GetGameState<AGBDungeonGameState>())
    {
        DungeonGameState->AddDungeonMapLoadedPlayer();
    }
}

void AGBDungeonGameMode::HandleDungeonMapLoadedCountChanged()
{
    AGBDungeonGameState* DungeonGameState = GetGameState<AGBDungeonGameState>();
    if (!DungeonGameState)
    {
        return;
    }

    const bool bAllReady = DungeonGameState->GetDungeonMapLoadedPlayerCount() >= GetExpectedPlayerCount();
    if (bAllReady && !DungeonGameState->GetMatchStarted())
    {
        DungeonGameState->SetMatchStarted(true);
        DungeonGameState->OnRep_MatchStarted(); // 서버에서도 즉시 처리  서버에서 할게 있나?

        StartMatch(); 

        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            if (APlayerController* PC = It->Get())
            {
                if (!PC->GetPawn()) { RestartPlayer(PC); }
            }
        }
    }
}

int32 AGBDungeonGameMode::GetActivePlayerCount() const
{
    int32 Count = 0;
    UWorld* World = GetWorld();
    if (!World)
    {
        return 0;
    }

    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
    {
        if (const APlayerController* PC = It->Get())
        {
            ++Count;
        }
    }
    return Count;
}

void AGBDungeonGameMode::RequestServerShutdown() const
{
    GB_BACKEND_LOG(Warning, TEXT("[DungeonGM] Shutting Down DS because No Players Remain"));

    // 소켓 정리
    if (UGBDedicatedSocketSubsystem* Socket = GetGameInstance()->GetSubsystem< UGBDedicatedSocketSubsystem>())
    {
        Socket->DisconnectSocket();
    }

    // 프로세스 kill
    FGenericPlatformMisc::RequestExit(false);
}

AGBDungeonGameState* AGBDungeonGameMode::GS() const
{
    return GetGameState<AGBDungeonGameState>();
}

int32 AGBDungeonGameMode::GetExpectedPlayerCount() const
{
    int32 Total = 0;
    for (const auto& KeyValuePair : ExpectedPlayerCountByTeam)
    {
        Total += KeyValuePair.Value;
    }
    return Total;
}

int32 AGBDungeonGameMode::GetConnectedPlayerCount() const
{
    int32 Total = 0;
    {
        for (const auto& KeyValuePair : ConnectedPlayerCountByTeam)
        {
            Total += KeyValuePair.Value;
        }
    }
    return Total;
}

UClass* AGBDungeonGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
    // 수정중 
    /*if (const AGBPlayerState* PS = InController ? InController->GetPlayerState<AGBPlayerState>() : nullptr)
    {
        FGBClassPresentation Row;
        if (GetClassPresentationFromPS(PS, Row))
        {
            GB_DEV_LOG(TEXT("[PawnSelect] Class=%d Pawn=%s"), (int32)PS->GetSelectedClass(), *GetNameSafe(Row.PawnClass.Get()));
            if (Row.PawnClass)
            {
                return Row.PawnClass.Get();
            }
        }
        else
        {
            GB_DEV_LOG(TEXT("[PawnSelect] No presentation for Class=%d"), (int32)PS->GetSelectedClass());
        }
    }*/

    return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void AGBDungeonGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    const int32 RemainingPlayers = GetActivePlayerCount();
    GB_BACKEND_LOG(Warning, TEXT("[DungeonGM] Logour : Remaining Players = %d"), RemainingPlayers);

    // 매치가 이미 시작된 상태고, 남은 플레이어가 없다면
    if (GetMatchState() == MatchState::InProgress && RemainingPlayers == 0)
    {
        GB_BACKEND_LOG(Warning, TEXT("[DungeonGM] No players left. Requesting server shutdown."));
        RequestServerShutdown();
    }
}

void AGBDungeonGameMode::RestartPlayer(AController* NewPlayer)
{
    // 팀 슬롯이 있으면 그 변환으로 스폰, 없으면 기본 로직
    if (const FString* TeamId = TeamIdByController.Find(NewPlayer))
    {
        FTransform Slot;
        if (PopNextSlotForTeam(*TeamId, Slot))
        {
            RestartPlayerAtTransform(NewPlayer, Slot);
            return;
        }
    }

    Super::RestartPlayer(NewPlayer);
}

void AGBDungeonGameMode::ArmCountdownForGameStart(float Seconds)
{
    // GameState에 초기 카운트다운 값 세팅
    if (AGBDungeonGameState* S = GS())
    {
        S->CountdownRemaining = Seconds;
    }

    // 혹시 기존 타이머 돌고 있으면 정리
    GetWorldTimerManager().ClearTimer(MatchStartCountdownTickHandle);

    // 1초마다 카운트다운 진행 & 조건 검사
    GetWorldTimerManager().SetTimer(
        MatchStartCountdownTickHandle,
        [this]()
        {
            AGBDungeonGameState* S = GS();
            if (!S)
            {
                // 월드가 종료 중이거나 GameState가 없는 특수 상황
                GetWorldTimerManager().ClearTimer(MatchStartCountdownTickHandle);
                return;
            }

            // 1) 남은 시간 감소
            S->CountdownRemaining = FMath::Max(0.f, S->CountdownRemaining - 1.f);

            // 2) 현재 인원 상황 확인
            const bool bEveryoneArrived =
                (GetConnectedPlayerCount() >= GetExpectedPlayerCount());

            // 3) 게임 시작 조건 체크
            const bool bTimeUp = (S->CountdownRemaining <= 0.f);

            if (bEveryoneArrived || bTimeUp)
            {
                // 카운트다운 종료
                GetWorldTimerManager().ClearTimer(MatchStartCountdownTickHandle);

                // 남은 시간을 0으로 정리(미관상 깔끔하게)
                S->CountdownRemaining = 0.f;
            }
        },
        1.0f,    // Interval: 1초마다
        true     // Looping: 반복 호출
    );
}

void AGBDungeonGameMode::CancelCountdownForGameStart()
{
    GetWorldTimerManager().ClearTimer(MatchStartCountdownTickHandle);

    if (AGBDungeonGameState* S = GS())
    {
        S->CountdownRemaining = 0.f;
    }
}
 
bool AGBDungeonGameMode::GetClassPresentationFromPS(const AGBPlayerState* PS, FGBClassPresentation& Out) const
{
    if (!PS || !ClassPresentationData)
    {
        return false;
    }
    return ClassPresentationData->Find(PS->GetSelectedClass(), Out); // DataAsset에 Find(ClassType, Out) 구현되어 있어야 함
}

TSubclassOf<APawn> AGBDungeonGameMode::ChoosePawnFromPS(const AGBPlayerState* PS) const
{
    FGBClassPresentation Row;
    return GetClassPresentationFromPS(PS, Row) ? Row.PawnClass : nullptr;
}

TSubclassOf<AHUD> AGBDungeonGameMode::ChooseHUDFromPS(const AGBPlayerState* PS) const
{
    FGBClassPresentation Row;
    return GetClassPresentationFromPS(PS, Row) ? Row.HUDClass : nullptr;
}

AGBCharacterSpawnPoint* AGBDungeonGameMode::ReserveAnchorForTeam(const FString& TeamId, int32 TeamSize)
{
    // 이미 예약돼 있으면 그냥 반환
    if (FTeamSpawnPointReservation* R = TeamSpawnPointReservations.Find(TeamId))
    {
        return R->Anchor.Get();
    }

    // 사용 가능한 앵커들 중에서 랜덤 선택(이미 점유된 건 제외)
    TArray<AGBCharacterSpawnPoint*> Candidates;
    for (auto& Weak : CharacterSpawnPool)
    {
        if (AGBCharacterSpawnPoint* SpawnPointActor = Weak.Get())
        {
            // 다른 팀이 앵커로 쓰고 있으면 skip
            bool bTaken = false;
            for (const auto& KV : TeamSpawnPointReservations)
            {
                if (KV.Value.Anchor.Get() == SpawnPointActor) 
                { 
                    bTaken = true; 
                    break;
                }
            }
            if (!bTaken)
            {
                Candidates.Add(SpawnPointActor);
            }
        }
    }
    if (Candidates.Num() == 0)
    {
        return nullptr;
    }

    AGBCharacterSpawnPoint* ChosenSpawnPoint = Candidates[FMath::RandRange(0, Candidates.Num() - 1)];

    // 슬롯 미리 계산(포인트 MaxSlots를 넘지 않도록)
    FTeamSpawnPointReservation R;
    R.Anchor = ChosenSpawnPoint;
    ChosenSpawnPoint->MakeSpawnTransforms(TeamSize, R.Slots);
    R.NextIndex = 0;

    TeamSpawnPointReservations.Add(TeamId, MoveTemp(R));
    return ChosenSpawnPoint;
}

bool AGBDungeonGameMode::PopNextSlotForTeam(const FString& TeamId, FTransform& Out)
{
    if (FTeamSpawnPointReservation* R = TeamSpawnPointReservations.Find(TeamId))
    {
        if (R->Slots.IsValidIndex(R->NextIndex))
        {
            Out = R->Slots[R->NextIndex++];
            GB_BACKEND_LOG(Warning, TEXT("PopNextSlotForTeam Succed for TeamId : %s"), *TeamId);
            return true;
        }
    }
    else
    {
        GB_BACKEND_LOG(Warning, TEXT("PopNextSlotForTeam Failed for TeamId : %s"), *TeamId);
    }

    return false;
}
