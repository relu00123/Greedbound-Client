// Fill out your copyright notice in the Description page of Project Settings.


#include "GB/UI/Lobby/PlayerList/GBLobbyPlayerGrid.h"
#include "Components/UniformGridPanel.h"
#include "UI/Lobby/PlayerList/GBLobbyPlayerEntryWidget.h"
#include "Components/UniformGridSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Subsystem/Lobby/GBLobbyUserManager.h"

void UGBLobbyPlayerGrid::Init(const TArray<FUserInfo>& UserList, EGBLobbyPlayerListType PlayerListType)
{
    if (!PlayerListPanel) return;

    PlayerListPanel->ClearChildren();
    SlotMap.Empty();
    UserSlotMap.Empty();
    PlayerGridType = PlayerListType;

    for (int32 i = 0; i < TotalSlots; ++i)
    {
        UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("WrapperCanvas"));
        UGBLobbyPlayerEntryWidget* Entry = CreateWidget<UGBLobbyPlayerEntryWidget>(this, PlayerEntryClass);

        if (!Entry || !Canvas) continue;
 
        Entry->SetLobbyPlayerListType(PlayerListType);

        if (UCanvasPanelSlot* CanvasSlot = Canvas->AddChildToCanvas(Entry))
        {
            CanvasSlot->SetAnchors(FAnchors(0.05f, 0.1f, 0.95f, 0.9f));
            CanvasSlot->SetOffsets(FMargin(0.f, 0.f, 0.f, 0.f));
        }

        Entry->SetEmptySlot();

        // 유저 정보 설정
        if (i < UserList.Num())
        {
            const FUserInfo& Info = UserList[i];
            Entry->SetupUserInfo(Info);
            UserSlotMap.Add(Info.NickName, i);
        }

        SlotMap.Add(i, Entry);

        const int32 Row = i / NumColumns;
        const int32 Col = i % NumColumns;

        if (UUniformGridSlot* GridSlot = PlayerListPanel->AddChildToUniformGrid(Canvas))
        {
            GridSlot->SetRow(Row);
            GridSlot->SetColumn(Col);

            GridSlot->SetHorizontalAlignment(HAlign_Fill);
            GridSlot->SetVerticalAlignment(VAlign_Fill);
        }

        SlotMap.Add(i, Entry);
    }

    if (UGBLobbyUserManager* LobbyUserManager = GetGameInstance()->GetSubsystem<UGBLobbyUserManager>())
    {
        OnUserClassChangedHandle = LobbyUserManager->BindOnUserClassChanged(FOnUserClassChanged::FDelegate::CreateUObject(
            this, &UGBLobbyPlayerGrid::HandleUserClassChanged));

        OnUserGamePhaseChangedHandle = LobbyUserManager->BindOnUserGamePhaseChanged(FOnUserGamePhaseChanged::FDelegate::CreateUObject(
            this, &UGBLobbyPlayerGrid::HandleUserGamePhaseChanged
        ));
    }
}

void UGBLobbyPlayerGrid::OnUserAdded(const FUserInfo& Info)
{
    if (int32* FoundIndex = UserSlotMap.Find(Info.NickName))
    {
        if (UGBLobbyPlayerEntryWidget* Entry = SlotMap.FindRef(*FoundIndex))
        {
            Entry->SetupUserInfo(Info); // 갱신
        }
        return;
    }

    // 새로운 유저인 경우 빈 슬롯 탐색
    for (auto& Pair : SlotMap)
    {
        int32 Index = Pair.Key;
        UGBLobbyPlayerEntryWidget* Entry = Pair.Value;

        if (Entry && Entry->IsEmptySlot())
        {
            Entry->SetupUserInfo(Info);
            UserSlotMap.Add(Info.NickName, Index);
            break;
        }
    }
}

void UGBLobbyPlayerGrid::OnUserUpdated(const FUserInfo& Info)
{
    if (int32* FoundIndex = UserSlotMap.Find(Info.NickName))
    {
        if (UGBLobbyPlayerEntryWidget* Entry = SlotMap.FindRef(*FoundIndex))
        {
            Entry->SetupUserInfo(Info); // 갱신
        }
        return;
    }

    // 새로운 유저인 경우 빈 슬롯 탐색
    /*for (auto& Pair : SlotMap)
    {
        int32 Index = Pair.Key;
        UGBLobbyPlayerEntryWidget* Entry = Pair.Value;

        if (Entry && Entry->IsEmptySlot())
        {
            Entry->SetupUserInfo(Info);
            UserSlotMap.Add(Info.NickName, Index);
            break;
        }
    }*/
}

void UGBLobbyPlayerGrid::OnUserRemoved(const FUserInfo& UserInfo)
{
    if (int32* FoundIndex = UserSlotMap.Find(UserInfo.NickName))
    {
        if (UGBLobbyPlayerEntryWidget* Entry = SlotMap.FindRef(*FoundIndex))
        {
            Entry->SetEmptySlot();
        }
        UserSlotMap.Remove(UserInfo.NickName);
    }

    ReorderEntries();
}

void UGBLobbyPlayerGrid::OnUserRemoved(const FString& UserName)
{
    if (int32* FoundIndex = UserSlotMap.Find(UserName))
    {
        if (UGBLobbyPlayerEntryWidget* Entry = SlotMap.FindRef(*FoundIndex))
        {
            Entry->SetEmptySlot();
        }
        UserSlotMap.Remove(UserName);
    }

    ReorderEntries();
}

void UGBLobbyPlayerGrid::HandleUserClassChanged(const FString& UserName, EGBCharacterClassType ChangedClassType)
{
    if (int32* FoundIndex = UserSlotMap.Find(UserName))
    {
        if (UGBLobbyPlayerEntryWidget* Entry = SlotMap.FindRef(*FoundIndex))
        {
            Entry->SetupUserClass(ChangedClassType); // 갱신
        }
        return;
    }
}

void UGBLobbyPlayerGrid::HandleUserGamePhaseChanged(const FString& UserName, EGBClientGamePhase ChangedGamePhase)
{
    if (int32* FoundIndex = UserSlotMap.Find(UserName))
    {
        if (UGBLobbyPlayerEntryWidget* Entry = SlotMap.FindRef(*FoundIndex))
        {
            Entry->SetupUserGamePhase(ChangedGamePhase); // 갱신
        }

        return;
    }
}

void UGBLobbyPlayerGrid::NativeDestruct()
{
    if (UGBLobbyUserManager* LobbyUserManager = GetGameInstance()->GetSubsystem<UGBLobbyUserManager>())
    {
        LobbyUserManager->UnbindOnUserClassChanged(OnUserClassChangedHandle);
        LobbyUserManager->UnbindOnUserGamePhaseChanged(OnUserGamePhaseChangedHandle);
    }

    Super::NativeDestruct();
}

void UGBLobbyPlayerGrid::ReorderEntries()
{
    TArray<FUserInfo> ConnectedUsers;

    // 1. 현재 접속중인 유저 목록 추출 (UserSlotMap 기준)
    for (const auto& Pair : UserSlotMap)
    {
        const FString& Nickname = Pair.Key;
        const int32 Index = Pair.Value;

        if (UGBLobbyPlayerEntryWidget* Entry = SlotMap.FindRef(Index))
        {
            if (!Entry->IsEmptySlot())
            {
                ConnectedUsers.Add(Entry->GetUserInfo()); // GetUserInfo()를 구현해야 함
            }
        }
    }

    // 2. 전체 슬롯을 모두 Empty로 초기화
    for (auto& Pair : SlotMap)
    {
        if (Pair.Value)
        {
            Pair.Value->SetEmptySlot();
        }
    }
    UserSlotMap.Empty();

    // 3. 앞에서부터 다시 유저 배치
    for (int32 i = 0; i < ConnectedUsers.Num(); ++i)
    {
        const FUserInfo& Info = ConnectedUsers[i];

        if (UGBLobbyPlayerEntryWidget* Entry = SlotMap.FindRef(i))
        {
            Entry->SetupUserInfo(Info);
            UserSlotMap.Add(Info.NickName, i);
        }
    }
}

//void UGBLobbyPlayerGrid::CreateAndAddEntry(const FUserInfo& Info)
//{
//    if (!PlayerEntryClass || !PlayerListPanel) return;
//    if (EntryMap.Contains(Info.NickName)) return; // 중복 방지
//
//    auto* Entry = CreateWidget<UGBLobbyPlayerEntryWidget>(this, PlayerEntryClass);
//    if (!Entry) return;
//
//    Entry->SetupUserInfo(Info);
//    EntryMap.Add(Info.NickName, Entry);
//
//    PlayerListPanel->AddChild(Entry);
//}
//
//void UGBLobbyPlayerGrid::RebuildGridLayout()
//{
//    if (!PlayerListPanel) return;
//
//    const int32 NumColumns = 4;
//    int32 Index = 0;
//
//    for (const auto& Pair : EntryMap)
//    {
//        if (!Pair.Value || !Pair.Value->IsValidLowLevel()) continue;
//
//        int32 Row = Index / NumColumns;
//        int32 Column = Index % NumColumns;
//
//        if (UUniformGridSlot* GridSlot = Cast<UUniformGridSlot>(Pair.Value->Slot))
//        {
//            GridSlot->SetRow(Row);
//            GridSlot->SetColumn(Column);
//        }
//
//        ++Index;
//    }
//}
