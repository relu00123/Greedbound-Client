// Fill out your copyright notice in the Description page of Project Settings.


#include "GBClientPhaseSubsystem.h"
#include "Subsystem/Network/Client/GBClientSocketRequestHelper.h"
#include "Subsystem/Matching/GBDungeonJoinSubsystem.h"
#include "Define/GBDefine.h"
#include "Game/GBGameInstance.h"

void UGBClientPhaseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    Collection.InitializeDependency<UGBDungeonJoinSubsystem>();

    UGBDungeonJoinSubsystem* DungeonJoinSubsystem = GetGameInstance()->GetSubsystem<UGBDungeonJoinSubsystem>();
    if (DungeonJoinSubsystem)
    {
        OnDungeonJoinStartedHandle = DungeonJoinSubsystem->BindOnJoinStarted(FDungeonJoinStarted::FDelegate::CreateUObject(this, &ThisClass::HandleOnDungeonJoinStarted));
    }
}

void UGBClientPhaseSubsystem::Deinitialize()
{
    UGBDungeonJoinSubsystem* DungeonJoinSubsystem = GetGameInstance()->GetSubsystem<UGBDungeonJoinSubsystem>();
    if (DungeonJoinSubsystem)
    {
        DungeonJoinSubsystem->UnbindOnJoinStarted(OnDungeonJoinStartedHandle);
    }

    Super::Deinitialize();
}

void UGBClientPhaseSubsystem::HandleGamePhaseChanged(bool bSuccess, EGBClientGamePhase beforeState, EGBClientGamePhase ChangedState)
{
    if (!bSuccess)
    {
        GB_BACKEND_LOG(Warning, TEXT("[GBClientPhaseSubsystem.cpp] HandleGamePhaseChange Failed"));
        return;
    }

    SetMyGamePhase(beforeState, ChangedState);
}

void UGBClientPhaseSubsystem::SetMyGamePhase(EGBClientGamePhase BeforePhase, EGBClientGamePhase NewPhase)
{
    FString BeforePhaseStr = StaticEnum<EGBClientGamePhase>()->GetNameStringByValue(static_cast<int64>(BeforePhase));
    FString NewPhaseStr = StaticEnum<EGBClientGamePhase>()->GetNameStringByValue(static_cast<int64>(NewPhase));
    FString CurrentPhaseStr = StaticEnum<EGBClientGamePhase>()->GetNameStringByValue(static_cast<int64>(MyGamePhase));

    GB_BACKEND_LOG(Warning, TEXT("SetMyGamepahsed Called!"));
    GB_BACKEND_LOG(Warning, TEXT("Current GamePhase : %s"), *CurrentPhaseStr);
    GB_BACKEND_LOG(Warning, TEXT("SetMyGamePhase Called from %s -> To %s"), *BeforePhaseStr, *NewPhaseStr);

    // 기존과 같다면 무시 
    if (MyGamePhase == NewPhase)
    {
        return; 
    }

    EGBClientGamePhase PreviousPhase = MyGamePhase;

    HandleExit(BeforePhase);
    MyGamePhase = NewPhase;
    HandleEtner(NewPhase);

    GB_BACKEND_LOG(Warning, TEXT("!!SetMyGamePhase Called from %s -> To %s"), *BeforePhaseStr, *NewPhaseStr);
  
    OnMyGamePhaseChanged.Broadcast(PreviousPhase, NewPhase);
}

void UGBClientPhaseSubsystem::SendGamePhaseChangeFromClient(EGBClientGamePhase NewPhase)
{
    FString NewPhaseStr = GamePhaseToString(NewPhase);

    // Ack를 받기 전까지 현재 상태를 Pending으로 둔다.
    MyGamePhase = EGBClientGamePhase::Pending;

    GBClientSocketRequestHelper::SendGamePhaseChangeRequest(NewPhaseStr);
}
 
void UGBClientPhaseSubsystem::HandleEtner(EGBClientGamePhase NewPhase)
{
    switch (NewPhase)
    {
    case EGBClientGamePhase::Lobby :
        break;
    default :
        break;
    }
}

void UGBClientPhaseSubsystem::HandleExit(EGBClientGamePhase OldPhase)
{
    switch (OldPhase)
    {
    case EGBClientGamePhase::Lobby:
        break;
    default:
        break;
    }
}

void UGBClientPhaseSubsystem::HandleOnDungeonJoinStarted(const FDungeonJoinInfo& DungeonInfo)
{
    // Phase상태 변경 (이미 서버에서 한번 해줬기에 중복이긴함)
    //SetMyGamePhase(MyGamePhase, EGBClientGamePhase::JoiningDungeon);
}

FString UGBClientPhaseSubsystem::GamePhaseToString(EGBClientGamePhase GamePhase)
{
    if (const UEnum* Enum = StaticEnum<EGBClientGamePhase>())
    {
        return Enum->GetNameStringByValue(static_cast<int64>(GamePhase));  // 안정적: 열거자 원본 이름
    }

    return TEXT("none");
}

bool  UGBClientPhaseSubsystem::StringToGamePhase(const FString& Str, EGBClientGamePhase& OutPhase)
{
    const UEnum* Enum = StaticEnum<EGBClientGamePhase>();
    if (!Enum) 
    { 
        OutPhase = EGBClientGamePhase::None; return false; 
    }

    // 1) 정확히 이름 매칭 (예: "Lobby", "InGame")
    int64 Val = Enum->GetValueByNameString(Str);
    if (Val != INDEX_NONE) 
    { 
        OutPhase = (EGBClientGamePhase)Val; return true;
    }

    // 2) 대소문자 무시 또는 DisplayName까지 허용
    const int32 NumEnums = Enum->NumEnums();
    for (int32 i = 0; i < NumEnums; ++i)
    {
#if WITH_EDITOR
        if (Enum->HasMetaData(TEXT("Hidden"), i))
        {
            continue;
        }
#endif

        const FString Name = Enum->GetNameStringByIndex(i); // "Lobby"
        if (Name.Equals(Str, ESearchCase::IgnoreCase))
        {
            OutPhase = (EGBClientGamePhase)Enum->GetValueByIndex(i); 
            return true;
        }

        const FString Display = Enum->GetDisplayNameTextByIndex(i).ToString(); // UI 표시명
        if (Display.Equals(Str, ESearchCase::IgnoreCase))
        {
            OutPhase = (EGBClientGamePhase)Enum->GetValueByIndex(i); 
            return true;
        }
    }

    OutPhase = EGBClientGamePhase::None;
    return false;
}
