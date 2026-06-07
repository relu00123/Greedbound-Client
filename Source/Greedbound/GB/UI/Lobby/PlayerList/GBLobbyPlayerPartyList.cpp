// Fill out your copyright notice in the Description page of Project Settings.


#include "GB/UI/Lobby/PlayerList/GBLobbyPlayerPartyList.h"
#include "UI/Lobby/PlayerList/GBLobbyPlayerEntryWidget.h"
#include "Subsystem/Social/GBPartyManager.h"
#include "Subsystem/Lobby/GBLobbyUserManager.h"
#include "Define/GBDefine.h"

void UGBLobbyPlayerPartyList::NativeConstruct()
{
    Super::NativeConstruct();

    // PartyMemberSlots 배열에 바인딩된 위젯들을 추가.
    if (FirstPartyMemberSlot)
    {
        PartyMemberSlots.Add(FirstPartyMemberSlot);
        FirstPartyMemberSlot->SetLobbyPlayerListType(EGBLobbyPlayerListType::Party);
    }

    if (SecondPartyMemberSlot)
    {
        PartyMemberSlots.Add(SecondPartyMemberSlot);
        SecondPartyMemberSlot->SetLobbyPlayerListType(EGBLobbyPlayerListType::Party);
    }

    // 처음에는 모든 파티원 슬롯을 숨깁니다.
    if (MySlot)
    {
        MySlot->SetEmptyPartySlot();
        MySlot->SetLobbyPlayerListType(EGBLobbyPlayerListType::Party);
    }

    for (TObjectPtr<UGBLobbyPlayerEntryWidget> PartyMemberSlot : PartyMemberSlots)
    {
        if (PartyMemberSlot)
        {
            PartyMemberSlot->SetEmptyPartySlot();
        }
    }

    BindingDelegates();

    // 이 시점에는 이미 LobbyUserManager의 정보들이 전부 갱신된 시점이다.
    // LobbyUserMangaer로부터 자기 자신에 대한 이름을 가져와 상태를 갱신해준다. 
    UGBLobbyUserManager* UserMgr = GetGameInstance()->GetSubsystem<UGBLobbyUserManager>();
    HandlePartyMemberJoined(UserMgr->GetMyUserInfo()->NickName);
}

void UGBLobbyPlayerPartyList::NativeDestruct()
{
    UnbindingDelegates();
    Super::NativeDestruct();
}

void UGBLobbyPlayerPartyList::BindingDelegates()
{
    UGBPartyManager* PartyMgr = GetGameInstance()->GetSubsystem<UGBPartyManager>();
    UGBLobbyUserManager* LobbyUserMgr = GetGameInstance()->GetSubsystem<UGBLobbyUserManager>();

    if (PartyMgr)
    {
        // !! (1)
        // PartyManager로 부터 PartyInfo가 Updated되었을때의 변경사항을 구독한다.
        PartyInfoUpdatedHandle = PartyMgr->BindOnPartyInfoUpdated(FOnPartyInfoUpdated::FDelegate::CreateUObject(this, &UGBLobbyPlayerPartyList::HandlePartyInfoUpdated));

        // !! (2)
        // PartyManager로부터 PartyMember가 추가되었을시를 구독한다.
        PartyJoinedHandle = PartyMgr->BindOnPartyMemberJoined(FOnPartyMemberJoined::FDelegate::CreateUObject(this, &ThisClass::HandlePartyMemberJoined));

        // !! (3)
        // PartyManager로부터 PartyMember가 탈퇴되었을시를 구독한다.
        PartyMemberLeftHandle = PartyMgr->BindOnPartyMemberLeft(FOnPartyMemberLeft::FDelegate::CreateUObject(this, &ThisClass::HandlePartyMemberLeft));

        // !! (4)
        // PartyManager로부터 Party를 탈퇴하였을시를 구독한다.
        PartyLeaveHandle = PartyMgr->BindOnLeaveFromParty(FOnLeaveFromParty::FDelegate::CreateUObject(this, &ThisClass::HandleLeaveFromParty));

        // !! (5)
        // PartyManager로부터 파티장이 바뀌었을시를 구독한다. 
        PartyHostTransferedHandle = PartyMgr->BindOnHostTransferred(FOnHostTransferred::FDelegate::CreateUObject(this, &ThisClass::HandlePartyHostTransfered));

        // !! (6)
        // PartyManger로 부터 파티원중 한명이 추방되었을때 들어오는 알림을 구독한다.
        PartyMemberKickedHandle = PartyMgr->BindOnPartyMemberKicked(FOnPartyMemberKicked::FDelegate::CreateUObject(this, &ThisClass::HandlePartyMemberKicked));

        // !! (7)
        // PartyManager로 부터 파티에서 강퇴되었을때 알림을 구독한다. 
        KickedFromPartyHandle = PartyMgr->BindOnKickedFromParty(FOnKickedFromParty::FDelegate::CreateUObject(this, &ThisClass::HandleKickedFromParty));
    }

    if (LobbyUserMgr)
    {
        // !! (8)
        UserClassChangedHandle = LobbyUserMgr->BindOnUserClassChanged(FOnUserClassChanged::FDelegate::CreateUObject(this, &ThisClass::HandleUserClassChanged));

        // !! (9)
        UserGamePhaseChangedHandle = LobbyUserMgr->BindOnUserGamePhaseChanged(FOnUserGamePhaseChanged::FDelegate::CreateUObject(this, &ThisClass::HandleUserGamePhaseChanged));
    }
}

void UGBLobbyPlayerPartyList::UnbindingDelegates()
{
    UGBPartyManager* PartyMgr = GetGameInstance()->GetSubsystem<UGBPartyManager>();
    UGBLobbyUserManager* LobbyUserMgr = GetGameInstance()->GetSubsystem<UGBLobbyUserManager>();

    if (PartyMgr)
    {
        // !! (1)
        PartyMgr->UnbindOnPartyInfoUpdated(PartyInfoUpdatedHandle);
        // !! (2)
        PartyMgr->UnbindOnPartyMemberJoined(PartyJoinedHandle);
        // !! (3)
        PartyMgr->UnbindOnPartyMemberLeft(PartyMemberLeftHandle);
        // !! (4)
        PartyMgr->UnbindOnLeaveFromParty(PartyLeaveHandle);
        // !! (5)
        PartyMgr->UnbindOnHostTransferred(PartyHostTransferedHandle);
        // !! (6)
        PartyMgr->UnbindOnPartyMemberKicked(PartyMemberKickedHandle);
        // !! (7)
        PartyMgr->UnbindOnKickedFromParty(KickedFromPartyHandle);
    }

    if (LobbyUserMgr)
    {
        // !! (8)
        LobbyUserMgr->UnbindOnUserClassChanged(UserClassChangedHandle);
        // !! (9)
        LobbyUserMgr->UnbindOnUserGamePhaseChanged(UserGamePhaseChangedHandle);
    }
}

void UGBLobbyPlayerPartyList::HandlePartyInfoUpdated(const FPartyInfo& PartyInfo)
{
    RebuildFromSnapshot(PartyInfo);
}

void UGBLobbyPlayerPartyList::HandlePartyMemberJoined(const FString& UserId)
{
    // 내 슬롯은 따로 처리
    if (UGBLobbyUserManager* Lobby = GetGameInstance()->GetSubsystem<UGBLobbyUserManager>())
    {
        if (const FUserInfo* My = Lobby->GetMyUserInfo())
        {
            // 여기 비교는 UserId로! (지금은 NickName 비교였음)
            if (UserId == My->NickName)
            {
                MySlot->SetupUserInfo(*My);
                MySlot->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
                return;
            }
        }
    }

    // 파티원 슬롯 배치/갱신
    AddOrUpdatePartyMemberVisual(UserId);
}

void UGBLobbyPlayerPartyList::HandlePartyMemberLeft(const FString& UserId)
{
    // 1. 해당 유저 슬롯 비우고 맵에서 제거
    ClearSlotByUser(UserId);

    // 2. 위로 정렬 (빈칸 압축)
    CompactPartyMemberSlots();
}

void UGBLobbyPlayerPartyList::HandlePartyMemberKicked(const FString& UserId)
{
    // 1. 해당 유저 슬롯 비우고 맵에서 제거
    ClearSlotByUser(UserId);

    // 2. 위로 정렬 (빈칸 압축)
    CompactPartyMemberSlots();
}

void UGBLobbyPlayerPartyList::HandleKickedFromParty()
{
    ClearAllPartyMemberSlots();
}

void UGBLobbyPlayerPartyList::HandleLeaveFromParty()
{
    // 파티장이였을 경우는 파티리더 아이콘을 빼줘야 한다. 
    if (MySlot)
    {
        MySlot->SetPartyLeaderIcon(false);
    }

    ClearAllPartyMemberSlots();
}

void UGBLobbyPlayerPartyList::HandleUserClassChanged(const FString& UserName, EGBCharacterClassType ChangedClassType)
{
    if (UserIdToSlot.Contains(UserName) || MySlot.Get()->GetUserInfo().NickName == UserName)
    {
        AddOrUpdatePartyMemberVisual(UserName);
    }
}

void UGBLobbyPlayerPartyList::HandleUserGamePhaseChanged(const FString& UserName, EGBClientGamePhase GamePhase)
{
    if (UserIdToSlot.Contains(UserName) || MySlot.Get()->GetUserInfo().NickName == UserName)
    {
        AddOrUpdatePartyMemberVisual(UserName);
    }
}

void UGBLobbyPlayerPartyList::HandleUserInfoChanged(const FUserInfo& UserInfo)
{
    // 표시 중인 멤버만 갱신
    FString UserId = UserInfo.NickName;

    if (UserIdToSlot.Contains(UserId))
    {
        AddOrUpdatePartyMemberVisual(UserId);
    }

    // 내 슬롯 갱신
    if (UGBLobbyUserManager* Lobby = GetGameInstance()->GetSubsystem<UGBLobbyUserManager>())
    {
        if (const FUserInfo* My = Lobby->GetMyUserInfo())
        {
            if (UserId == My->NickName)
            {
                MySlot->SetupUserInfo(*My);
            }
        }
    }
}

void UGBLobbyPlayerPartyList::HandlePartyHostTransfered(int32 PartyId, const FString& OldHostName, const FString& NewHostName)
{
    MarkPartyHost(NewHostName);
}

void UGBLobbyPlayerPartyList::AddOrUpdatePartyMemberVisual(const FString& UserId)
{
    UGBLobbyUserManager* Lobby = GetGameInstance()->GetSubsystem<UGBLobbyUserManager>();

    if (!Lobby)
    {
        return;
    }

    // 0. 나 자신이면 MySlot만 갱신하고 종료 
    if (const FUserInfo* My = Lobby->GetMyUserInfo())
    {
        if (UserId == My->NickName)  // ※ 닉네임 말고 UserId 권장
        {
            if (MySlot) FillSlot(MySlot, *My);    // 필요하면 FillMySlot로 분리
            return;
        }
    }

    // 1. 이미 표시 중이면 갱신
    if (TObjectPtr<UGBLobbyPlayerEntryWidget>* Found = UserIdToSlot.Find(UserId))
    {
        const FUserInfo* Info = Lobby->FindUser(UserId);

        if (Info)
        {
            FillSlot(*Found, *Info);
        }

        return;
    }

    // 2. 빈 파티원 슬롯에 신규 배치
    if (UGBLobbyPlayerEntryWidget* EmptySlot = FindEmptySlot())
    {
        const FUserInfo* Info = (Lobby->FindUser(UserId));

        if (Info)
        {
            FillSlot(EmptySlot, *Info);
        }

        UserIdToSlot.Add(UserId, EmptySlot);
    }
}

void UGBLobbyPlayerPartyList::FillSlot(UGBLobbyPlayerEntryWidget* TargetSlot, const FUserInfo& Info)
{
    if (!TargetSlot)
    {
        return;
    }
    TargetSlot->SetupUserInfo(Info); // 닉네임/클래스/온라인 등 채우는 함수(이미 있으신 듯)
    TargetSlot->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    TargetSlot->InvalidateLayoutAndVolatility(); // 필요시
}

void UGBLobbyPlayerPartyList::ClearSlotByUser(const FString& UserId)
{
    if (TObjectPtr<UGBLobbyPlayerEntryWidget>* Found = UserIdToSlot.Find(UserId))
    {
        if (*Found)
        {
            (*Found)->SetEmptyPartySlot();
        }

        UserIdToSlot.Remove(UserId);
    }
}

void UGBLobbyPlayerPartyList::ClearAllPartyMemberSlots()
{
    for (UGBLobbyPlayerEntryWidget* PartyMemberSlot : PartyMemberSlots)
    {
        if (PartyMemberSlot)
        {
            PartyMemberSlot->SetEmptyPartySlot();
        }
    }

    UserIdToSlot.Empty();
}

void UGBLobbyPlayerPartyList::RebuildFromSnapshot(const FPartyInfo& PartyInfo)
{
    // 1. 모두 초기화
    for (auto& P : PartyMemberSlots)
    {
        if (P)
        {
            P->SetEmptyPartySlot();
        }
    }

    UserIdToSlot.Reset();

    // 2. 파티원 슬롯 채우기
    if (UGBLobbyUserManager* Lobby = GetGameInstance()->GetSubsystem<UGBLobbyUserManager>())
    {
        for (const FString& Member : PartyInfo.Members)
        {
            if (const FUserInfo* Info = Lobby->FindUser(Member))
            {
                AddOrUpdatePartyMemberVisual(Member);
            }
        }
    }

    // 호스트 마크 표시
    MarkPartyHost(PartyInfo.HostName);
}

void UGBLobbyPlayerPartyList::MarkPartyHost(const FString& HostUserId)
{
    // 모든 슬롯에게서 파티장 표식을 뺏어야 한다. 

    //  내 슬롯
    if (UGBLobbyUserManager* Lobby = GetGameInstance()->GetSubsystem<UGBLobbyUserManager>())
    {
        if (const FUserInfo* MyUserInfo = Lobby->GetMyUserInfo())
        {
            MySlot->SetPartyLeaderIcon(MyUserInfo->NickName == HostUserId);

            // 파티원 슬롯들
            for (auto& Pair : UserIdToSlot)
            {
                if (Pair.Value)
                {
                    Pair.Value->SetPartyLeaderIcon(Pair.Key == HostUserId);
                }
            }
        }
    }
}

void UGBLobbyPlayerPartyList::CompactPartyMemberSlots()
{
    // 1) 현재 채워진 항목을 왼→오 순서대로 수집
    struct FEntrySnapshot
    {
        FString          UserId;
        FUserInfo        Info;
        bool             bIsHost = false;
    };

    TArray<FEntrySnapshot> Filled;
    Filled.Reserve(PartyMemberSlots.Num());

    for (UGBLobbyPlayerEntryWidget* PartyMemberSlot : PartyMemberSlots)
    {
        if (!PartyMemberSlot || PartyMemberSlot->IsEmptyPartySlot())
        {
            continue;
        }

        FEntrySnapshot Snap;
        Snap.UserId = PartyMemberSlot->GetUserInfo().NickName;
        Snap.Info = PartyMemberSlot->GetUserInfo();
        Snap.bIsHost = PartyMemberSlot->IsPartyHost();
        Filled.Add(MoveTemp(Snap));
    }

    // 2) 전부 비우기 + 매핑도 리셋
    for (UGBLobbyPlayerEntryWidget* PartyMemberSlot : PartyMemberSlots)
    {
        if (PartyMemberSlot)
        {
            PartyMemberSlot->SetEmptyPartySlot();
        }
    }
    UserIdToSlot.Empty();

    // 3) 왼쪽부터 다시 채우기(시각적 순서 유지)
    int32 WriteIndex = 0;
    for (const FEntrySnapshot& Snap : Filled)
    {
        if (!PartyMemberSlots.IsValidIndex(WriteIndex))
        {
            break;
        }

        UGBLobbyPlayerEntryWidget* PartyMemberSlot = PartyMemberSlots[WriteIndex];
        if (!PartyMemberSlot)
        {
            continue;
        }

        FillSlot(PartyMemberSlot, Snap.Info);         // 기존 FillSlot 재사용
        UserIdToSlot.Add(Snap.UserId, PartyMemberSlot);

        if (Snap.bIsHost)
        {
            PartyMemberSlot->SetPartyLeaderIcon(true);         // 필요: EntryWidget에 SetIsHost(bool)
        }

        ++WriteIndex;
    }
}

UGBLobbyPlayerEntryWidget* UGBLobbyPlayerPartyList::FindEmptySlot() const
{
    for (TObjectPtr<UGBLobbyPlayerEntryWidget> TargetSlot : PartyMemberSlots)
    {
        if (TargetSlot && TargetSlot->IsEmptyPartySlot())
        {
            return TargetSlot;
        }
    }
    return nullptr;
}


