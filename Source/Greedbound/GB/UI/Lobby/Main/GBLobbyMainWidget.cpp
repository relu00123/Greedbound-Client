// Fill out your copyright notice in the Description page of Project Settings.


#include "GB/UI/Lobby/Main/GBLobbyMainWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Subsystem/Network/Client/GBClientSessionSubsystem.h"
#include "Subsystem/Network/Client/GBClientHttpManagerSubsystem.h"
#include "Subsystem/Map/GBMapSelectionSubsystem.h"
#include "Define/GBDefine.h"
#include "Subsystem/Social/GBPartyManager.h"
#include "GBLobbyMatchStartButton.h"
#include "Subsystem/Matching/GBClientMatchQueueSubsystem.h"
#include "Subsystem/GamePhase/GBClientPhaseSubsystem.h"

void UGBLobbyMainWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Match State 초기화
    MatchState = EGBLobbyMatchQueueState::SoloIdle;

    // MatchStartButton Handlers 세팅
    const int32 NumStates = static_cast<int32>(EGBLobbyMatchQueueState::END);
    MatchStartButtonHandlers.SetNum(NumStates);

    MatchStartButtonHandlers[(int32)EGBLobbyMatchQueueState::PartyHostIdle] = [this] {MatchStartButtonClicked_At_PartyHostIdle(); };
    MatchStartButtonHandlers[(int32)EGBLobbyMatchQueueState::PartyHostSearching] = [this] {MatchStartButtonClicked_At_PartyHostSearching(); };
    MatchStartButtonHandlers[(int32)EGBLobbyMatchQueueState::PartyMemberIdle] = [this] {MatchStartButtonClicked_At_PartyMemberIdle(); };
    MatchStartButtonHandlers[(int32)EGBLobbyMatchQueueState::PartyMemberSearching] = [this] {MatchStartButtonClicked_At_PartyMemberSearching(); };
    MatchStartButtonHandlers[(int32)EGBLobbyMatchQueueState::SoloIdle] = [this] {MatchStartButtonClicked_At_SoloIdle(); };
    MatchStartButtonHandlers[(int32)EGBLobbyMatchQueueState::SoloSearching] = [this] {MatchStartButtonClicked_At_SoloSearching(); };
    MatchStartButtonHandlers[(int32)EGBLobbyMatchQueueState::MatchAssigned] = [this] {MatchStartButtonClicked_At_MatchAssigned(); };
    MatchStartButtonHandlers[(int32)EGBLobbyMatchQueueState::Pending] = [this] {MatchStartButtonClicked_At_Pending(); };

    if (MatchStartButton)
    {
        OnGameStartClickedHandle =  MatchStartButton->BindOnMatchStartClicked(FOnMatchStartClicked::FDelegate::CreateUObject(this, &UGBLobbyMainWidget::OnGameStartClicked));
        MatchStartButton->ApplyState(EGBLobbyMatchQueueState::SoloIdle);
    }

    UGBPartyManager* PartyMgr =  GetGameInstance()->GetSubsystem<UGBPartyManager>();
    if (PartyMgr)
    {
        OnMyPartyRoleChangedHandle = PartyMgr->BindOnMyPartyRoleChanged(FOnMyPartyRoleChanged::FDelegate::CreateUObject(this, &ThisClass::HandleOnMyPartyRoleChanged));
    }

    UGBClientMatchQueueSubsystem* MatchQueueSubsystem = GetGameInstance()->GetSubsystem<UGBClientMatchQueueSubsystem>();
    if (MatchQueueSubsystem)
    {
        OnMatchQueueCanceledHandle = MatchQueueSubsystem->BindOnQueueCanceled(FOnQueueCanceled::FDelegate::CreateUObject(this, &ThisClass::HandleOnMatchQueueCanceled));
        OnMatchQueueJoinedHandle = MatchQueueSubsystem->BindOnQueueJoined(FOnQueueJoined::FDelegate::CreateUObject(this, &ThisClass::HandleOnMatchQueueJoined));
    }

    UGBClientPhaseSubsystem* ClientPhaseSubsystem = GetGameInstance()->GetSubsystem<UGBClientPhaseSubsystem>();
    if (ClientPhaseSubsystem)
    {
        OnMyGamePhaseChangedHandle = ClientPhaseSubsystem->BindOnClientGamePhaseChanged(FOnMyGamePhaseChanged::FDelegate::CreateUObject(this, &ThisClass::HandleOnMyGamePhaseChanged));
    }
}

void UGBLobbyMainWidget::NativeDestruct()
{
    UGBPartyManager* PartyMgr = GetGameInstance()->GetSubsystem<UGBPartyManager>();
    if (PartyMgr)
    {
        PartyMgr->UnbindOnMyPartyRoleChanged(OnMyPartyRoleChangedHandle);
    }

    if (MatchStartButton)
    {
        MatchStartButton->UnbindOnMatchStartClicked(OnGameStartClickedHandle);
    }

    UGBClientMatchQueueSubsystem* MatchQueueSubsystem = GetGameInstance()->GetSubsystem<UGBClientMatchQueueSubsystem>();
    if (MatchQueueSubsystem)
    {
        MatchQueueSubsystem->UnbindOnQueueCanceled(OnMatchQueueCanceledHandle);
        MatchQueueSubsystem->UnbindOnQueueJoined(OnMatchQueueJoinedHandle);
    }

    UGBClientPhaseSubsystem* ClientPhaseSubsystem = GetGameInstance()->GetSubsystem<UGBClientPhaseSubsystem>();
    if (ClientPhaseSubsystem)
    {
        ClientPhaseSubsystem->UnbindOnClientGamePhaseChanged(OnMyGamePhaseChangedHandle);
    }

    Super::NativeDestruct();
}

void UGBLobbyMainWidget::HandleOnMatchQueueCanceled(const FClientQueueState& QueueState)
{
    // 파티 큐 였을 경우 (Host였는지, 멤버였는지), 솔로큐 였을 경우에 따라 분기해야함. 
    UGBClientMatchQueueSubsystem* MatchQueueSubsystem = GetGameInstance()->GetSubsystem<UGBClientMatchQueueSubsystem>();
    if (MatchQueueSubsystem && MatchStartButton)
    {
        switch (MatchQueueSubsystem->GetLobbyMatchQueueState())
        {
        case EGBLobbyMatchQueueState::PartyHostIdle:
            MatchStartButton->ApplyState(EGBLobbyMatchQueueState::PartyHostIdle);
            break;
        case EGBLobbyMatchQueueState::PartyMemberIdle:
            MatchStartButton->ApplyState(EGBLobbyMatchQueueState::PartyMemberIdle);
            break;
        case EGBLobbyMatchQueueState::SoloIdle:
            MatchStartButton->ApplyState(EGBLobbyMatchQueueState::SoloIdle);
            break;
        default:
            break;
        }
    }
}

void UGBLobbyMainWidget::HandleOnMatchQueueJoined(const FClientQueueState& QueueState)
{
    UGBClientMatchQueueSubsystem* MatchQueueSubsystem = GetGameInstance()->GetSubsystem<UGBClientMatchQueueSubsystem>();
    if (MatchQueueSubsystem && MatchStartButton)
    {
        switch (MatchQueueSubsystem->GetLobbyMatchQueueState())
        {
        case EGBLobbyMatchQueueState::PartyHostSearching:
            MatchStartButton->ApplyState(EGBLobbyMatchQueueState::PartyHostSearching);
            break;
        case EGBLobbyMatchQueueState::PartyMemberSearching:
            MatchStartButton->ApplyState(EGBLobbyMatchQueueState::PartyMemberSearching);
            break;
        case EGBLobbyMatchQueueState::SoloSearching:
            MatchStartButton->ApplyState(EGBLobbyMatchQueueState::SoloSearching);
            break;
        default:
            break;
        }
    }
}

void UGBLobbyMainWidget::MatchStartButtonClicked_At_PartyHostIdle()
{
    GB_BACKEND_LOG(Warning, TEXT("MatchStartButton Clicked : PartyHostIdle! "));

    UGBClientMatchQueueSubsystem* MatchQueueSubsystem = GetGameInstance()->GetSubsystem<UGBClientMatchQueueSubsystem>();
    if (MatchQueueSubsystem)
    {
        MatchQueueSubsystem->RequestMatchQueueJoin();

        // UI단에서 바로 Button 비활성화 
        if (MatchStartButton)
        {
            MatchStartButton->ApplyState(EGBLobbyMatchQueueState::Pending);
        }
    }
}

void UGBLobbyMainWidget::MatchStartButtonClicked_At_PartyHostSearching()
{
    GB_BACKEND_LOG(Warning, TEXT("MatchStartButton Clicked : PartyHostSearching! "));

    // 서버 한테 취소해 달라고 메세지를 보내야 한다.

    UGBClientMatchQueueSubsystem* MatchQueueSubsystem = GetGameInstance()->GetSubsystem<UGBClientMatchQueueSubsystem>();
    if (MatchQueueSubsystem)
    {
        MatchQueueSubsystem->RequestMatchQueueCancel();

        // UI단계에서 바로 Butotn 비활성화
        if (MatchStartButton)
            MatchStartButton->ApplyState(EGBLobbyMatchQueueState::Pending);
    }

}

void UGBLobbyMainWidget::MatchStartButtonClicked_At_PartyMemberIdle()
{
    GB_BACKEND_LOG(Warning, TEXT("MatchStartButton Clicked : PartyMemberIdle! "));
}

void UGBLobbyMainWidget::MatchStartButtonClicked_At_PartyMemberSearching()
{
    GB_BACKEND_LOG(Warning, TEXT("MatchStartButton Clicked : PartyMemberSearching! "));
}

void UGBLobbyMainWidget::MatchStartButtonClicked_At_SoloIdle()
{
    GB_BACKEND_LOG(Warning, TEXT("MatchStartButton Clicked : SoloIdle! "));

    UGBClientMatchQueueSubsystem* MatchQueueSubsystem = GetGameInstance()->GetSubsystem<UGBClientMatchQueueSubsystem>();
    if (MatchQueueSubsystem)
    {
        MatchQueueSubsystem->RequestMatchQueueJoin();

        // UI단에서 바로 Button 비활성화 
        if (MatchStartButton)
        {
            MatchStartButton->ApplyState(EGBLobbyMatchQueueState::Pending);
        }
    }
}

void UGBLobbyMainWidget::MatchStartButtonClicked_At_SoloSearching()
{
    GB_BACKEND_LOG(Warning, TEXT("MatchStartButton Clicked : SoloSearching! "));

    UGBClientMatchQueueSubsystem* MatchQueueSubsystem = GetGameInstance()->GetSubsystem<UGBClientMatchQueueSubsystem>();
    if (MatchQueueSubsystem)
    {
        MatchQueueSubsystem->RequestMatchQueueCancel();

        // UI단계에서 바로 Butotn 비활성화
        if (MatchStartButton)
            MatchStartButton->ApplyState(EGBLobbyMatchQueueState::Pending);
    }
}

void UGBLobbyMainWidget::MatchStartButtonClicked_At_MatchAssigned()
{
    GB_BACKEND_LOG(Warning, TEXT("MatchStartButton Clicked : Match Assigned! "));
}

void UGBLobbyMainWidget::MatchStartButtonClicked_At_Pending()
{
    GB_BACKEND_LOG(Warning, TEXT("MatchStartButton Clicked : Pending! "));
}

void UGBLobbyMainWidget::HandleOnMyPartyRoleChanged(EPartyRole NewPartyRole)
{
    GB_BACKEND_LOG(Warning, TEXT("Party Role Changed!! %s"), *UEnum::GetValueAsString(NewPartyRole));

    if (MatchStartButton)
    {
        switch (NewPartyRole)
        {
        case EPartyRole::Solo:
            MatchStartButton->ApplyState(EGBLobbyMatchQueueState::SoloIdle);
            break;
        case EPartyRole::Member:
            MatchStartButton->ApplyState(EGBLobbyMatchQueueState::PartyMemberIdle);
            break;
        case EPartyRole::Host:
            MatchStartButton->ApplyState(EGBLobbyMatchQueueState::PartyHostIdle);
            break;
        default:
            break;
        }
    }
}

void UGBLobbyMainWidget::HandleOnMyGamePhaseChanged(EGBClientGamePhase From, EGBClientGamePhase To)
{
    // 모든 게임 Phase에 대응하는 것은 아님. 
    if (MatchStartButton)
    {
        switch (To)
        {
        case EGBClientGamePhase::MatchAssigned:
            MatchStartButton->ApplyState(EGBLobbyMatchQueueState::MatchAssigned); 
            break;
        case EGBClientGamePhase::JoiningDungeon:
            break;

        default:
            break;
        }
    }
}

void UGBLobbyMainWidget::OnGameStartClicked()
{
    GB_BACKEND_LOG(Warning, TEXT("On GameStart Clicked!!"));

    UGBClientMatchQueueSubsystem* MatchQueueSubsystem = GetGameInstance()->GetSubsystem<UGBClientMatchQueueSubsystem>();
    if (MatchQueueSubsystem)
    {
        EGBLobbyMatchQueueState MatchQueueState = MatchQueueSubsystem->GetLobbyMatchQueueState();

        const int32 Idx = static_cast<int32>(MatchQueueState);

        if (MatchStartButtonHandlers.IsValidIndex(Idx) && MatchStartButtonHandlers[Idx])
        {
            MatchStartButtonHandlers[Idx]();
        }
    }

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

 

 
 
