// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/Social/GBPartyManager.h"
#include "Subsystem/Lobby/GBLobbyUserManager.h"
#include "Async/Async.h"
#include "Subsystem/Lobby/GBLobbyInitSubsystem.h"
#include "Subsystem/Network/Client/GBClientSessionSubsystem.h"
#include "Subsystem/Network/Client/GBClientSocketRequestHelper.h"
#include "Subsystem/UI/GBPopupManagerSubsystem.h"
#include "UI/Popup/Utils/GBPopupConfig.h"
#include "Define/GBDefine.h"


 
void UGBPartyManager::HandleOnUserRemoved(const FUserInfo& UserInfo)
{
    if (CurrentPartyInfo.Members.Contains(UserInfo.NickName))
    {
        CurrentPartyInfo.Members.Remove(UserInfo.NickName);
        OnPartyMemberLeft.Broadcast(UserInfo.NickName);
    }
}

void UGBPartyManager::Initialize(FSubsystemCollectionBase& Collection)
{
    // 이 Subsystem이 시작되기 전에 반드시 LobbyUserManager가 초기화 되도록 보장 

    Collection.InitializeDependency<UGBLobbyUserManager>();

    Super::Initialize(Collection);

    BindingDelegates();

    MyPartyRoleCached = EPartyRole::Solo; 
}

void UGBPartyManager::Deinitialize()
{
    UnbindingDelegates();

    Super::Deinitialize();
}

//void UGBPartyManager::OnMyUserInfoReady_Adapter(const FUserInfo& My)
//{
//    HandlePartyMemberJoined(My.NickName);
//}

void UGBPartyManager::TryPresentNextInvite()
{
    if (bInvitePopupActive)
        return; // 이미 팝업이 활성화됨

    if (PendingInvites.Num() == 0)
        return;

    const FPartyInvite& Active = PendingInvites[0]; // front
    bInvitePopupActive = true;

    OnActivePartyInviteChanged.Broadcast(Active);
}

 
void UGBPartyManager::HandlePartyInfoNotification(const FPartyInfo& PartyInfo)
{
    // UI 및 기타 시스템에 파티 정보가 업데이트되었음을 알립니다.
    OnPartyInfoUpdated.Broadcast(CurrentPartyInfo);

    GB_BACKEND_LOG(Warning, TEXT("PartyInfo Updated for PartyId : %d, Host : %s, Members : %s"),
        CurrentPartyInfo.PartyId, *CurrentPartyInfo.HostName, *FString::Join(CurrentPartyInfo.Members, TEXT(", ")));
}

void UGBPartyManager::HandlePartyJoinedByMe(const FPartyInfo& PartyInfo)
{
    CurrentPartyInfo = PartyInfo;

    // UI 및 기타 시스템에 파티 정보가 업데이트되었음을 알린다. 
    OnPartyInfoUpdated.Broadcast(CurrentPartyInfo); 
    UpdateMyPartyRoleAndBroadCastIfChanged();

  /*  UGBLobbyUserManager* LobbyMgr = GetGameInstance()->GetSubsystem<UGBLobbyUserManager>();

    if (!LobbyMgr)
    {
        return;
    }

    const FUserInfo* MyUserInfo = LobbyMgr->GetMyUserInfo();

    if (!MyUserInfo)
    {
        return;
    }*/

   /* if (MyUserInfo->NickName == CurrentPartyInfo.HostName)
    {
        OnMyPartyRoleChanged.Broadcast(EPartyRole::Host);
    }

    else
    {
        OnMyPartyRoleChanged.Broadcast(EPartyRole::Member);
    }*/
}

void UGBPartyManager::HandlePartyInviteReceived(const FString& inviterName)
{
    FPartyInvite invite;
    invite.InviterName = inviterName;
    invite.ReceivedAt = FDateTime::UtcNow(); // 이쪽 로직은 좀 수정할 필요가 있을 것 같음. 팝업이 뜬 시간부터 5초가 되어야 할 듯..?

    PendingInvites.Add(invite);

    // 바로 popup에 Broadcast하지 않고, 현재 파티 초대 팝업이 없을 경우에만 Broadcast하도록 한다. 
    TryPresentNextInvite();

}

void UGBPartyManager::HandlePartyMemberKickedReceived(const FString& KickedMemberName)
{
    UGBLobbyUserManager* LobbyMgr = GetGameInstance()->GetSubsystem<UGBLobbyUserManager>(); 

    if (CurrentPartyInfo.Members.Contains(KickedMemberName))
    {
        bool bAmIKicked = (LobbyMgr->GetMyUserInfo()->NickName == KickedMemberName);

        // 내가 강퇴당한 경우
        if (bAmIKicked)
        {
            // 파티 정보 리셋 
            CurrentPartyInfo.Reset();
            OnKickedFromParty.Broadcast();
            UpdateMyPartyRoleAndBroadCastIfChanged();
        }

        // 파티원이 강퇴당한 경우 
        else
        {
            CurrentPartyInfo.Members.Remove(KickedMemberName);
            OnPartyMemberKicked.Broadcast(KickedMemberName);
        }
    }
}

void UGBPartyManager::HandleSendPartyInviteResponse(bool bSuccess, TOptional<FString> Error, TOptional<FString> InviteeName)
{
    if (bSuccess)
    {
        UE_LOG(LogTemp, Warning, TEXT("[GBPartyManager.cpp] Party Invite Succeeded!! "));
    }

    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[GBPartyManager.cpp] Party Invite Failed!! "));

        FGBPopupConfig config;

        config.bUseCancel = false;
        config.bUseTimeout = false;

        const FString ErrorMsg = Error.IsSet() ? Error.GetValue() : TEXT("Unknown error");
        const FString Inviteename = InviteeName.IsSet() ? InviteeName.GetValue() : TEXT("Unknown");

        if (ErrorMsg == TEXT("INVITEE_NOT_FOUND"))
        {
            config.Message = FString::Printf(TEXT("%s not found"), *Inviteename);
        }

        else if (ErrorMsg == TEXT("NOT_AUTHENTICATED"))
        {
            config.Message = FString::Printf(TEXT("not authenticated error"));
        }

        else if (ErrorMsg == TEXT("INVITEE_ALREADY_IN_PARTY"))
        {
            config.Message = FString::Printf(TEXT("%s is already in other party"), *Inviteename);
        }

        else
        {
            config.Message = FString::Printf(TEXT("Unknown Error"));
        }

      /*  config.OnConfirm = [WeakGI = TWeakObjectPtr<UGameInstance>(GetGameInstance())]()
            {
                
            };*/

        UGBPopupManagerSubsystem* PopupManager = GetGameInstance()->GetSubsystem<UGBPopupManagerSubsystem>();
        if (PopupManager)
        {
            PopupManager->ShowPopup(config);
        }

    }
    
    // 파티초대 Popup을 띄우자. 

    //if (!bSuccess) {
    //    // 에러 메시지가 있으면 사용, 없으면 기본 메시지
    //    const FString Msg = Error.IsSet() ? Error.GetValue() : TEXT("Unknown error");
    //    // UI 토스트/로그 등
    //}
    //else {
    //    const FString Name = InviteeName.IsSet() ? InviteeName.GetValue() : TEXT("Unknown");
    //    // 성공 처리
    //}
}
 

/**
 * @brief 서버로부터 새로운 멤버가 파티에 들어왔다는 알림을 받고 내부 상태를 업데이트합니다.
 * 이 함수는 "PartyMemberJoined" 패킷에 대응됩니다.
 * @param NewMemberName 새로 들어온 멤버의 이름
 */
void UGBPartyManager::HandlePartyMemberJoined(const FString& NewMemberName)
{
    // 멤버 목록에 새로운 멤버를 추가합니다. 이미 존재할 경우 중복 추가를 방지합니다.
    if (!CurrentPartyInfo.Members.Contains(NewMemberName))
    {
        CurrentPartyInfo.Members.Add(NewMemberName);
        UE_LOG(LogTemp, Warning, TEXT("New member joined Party %s"), *NewMemberName);
    }

    // UI에 멤버가 추가되었음을 알립니다.
    OnPartyMemberJoined.Broadcast(NewMemberName);
}

void UGBPartyManager::BindingDelegates()
{
    // 여기서 부터는 LobbyUserManager를 안전하게 참조할 수 있다. 
    UGBLobbyUserManager* LobbyMgr = GetGameInstance()->GetSubsystem<UGBLobbyUserManager>();

   // 나 자신은 파티에 항상 추가되어 있도록 한다.  (이쪽 로직 틀림.. LobbyUserManager를 가져오고 한참뒤에 MyUserInfo가 체워진다.) 
    if (LobbyMgr)
    {
        // LobbyMgr->BindOnUserRemoved(FOnUserRemoved::FDelegate::CreateUObject(this, &UGBPartyManager::HandleOnUserRemoved));

    }
}

void UGBPartyManager::UnbindingDelegates()
{

}

EPartyRole UGBPartyManager::ComputeMyPartyRole() const
{
    UGBLobbyUserManager* LobbyUserManager =  GetGameInstance()->GetSubsystem<UGBLobbyUserManager>();

    if (!LobbyUserManager)
        return EPartyRole::NONE;

    if (!CurrentPartyInfo.IsValid())
        return EPartyRole::Solo;

    if (LobbyUserManager->GetMyUserName() == CurrentPartyInfo.HostName)
        return EPartyRole::Host;

    else
        return EPartyRole::Member;
}

void UGBPartyManager::UpdateMyPartyRoleAndBroadCastIfChanged()
{
    const EPartyRole NewRole = ComputeMyPartyRole();

    if (NewRole != MyPartyRoleCached)
    {
        MyPartyRoleCached = NewRole;
        OnMyPartyRoleChanged.Broadcast(NewRole);
    }
}

 
/**
 * @brief 서버로부터 멤버가 파티를 나갔다는 알림을 받고 내부 상태를 업데이트합니다.
 * 이 함수는 "PartyMemberLeft" 패킷에 대응됩니다.
 * @param PartyId 업데이트가 발생한 파티 ID
 * @param MemberName 나간 멤버의 이름
 */
void UGBPartyManager::HandlePartyMemberLeft(int32 PartyId, const FString& MemberName)
{
     UGBLobbyUserManager* LobbyMgr = GetGameInstance()->GetSubsystem<UGBLobbyUserManager>(); 
    bool bAmILeft = (LobbyMgr->GetMyUserInfo()->NickName == MemberName);


    UE_LOG(LogTemp, Warning, TEXT("[PartyManager.cpp] HandlePartyMemberLeft Called!"));

    // 자기 자신이 파티에서 나간 경우 
    if (bAmILeft)
    {
        UE_LOG(LogTemp, Warning, TEXT("[PartyManager.cpp] I left!"));
        CurrentPartyInfo.Reset();
        OnLeaveFromParty.Broadcast();
        UpdateMyPartyRoleAndBroadCastIfChanged();
    }

    // 다른 사람이 파티에서 나간 경우 
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[PartyManager.cpp] Other Member Left!"));
        // 멤버 목록에서 나간 멤버를 제거합니다. 
        int32 RemovedCount = CurrentPartyInfo.Members.RemoveSingle(MemberName);

        if (RemovedCount > 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("Member left Party %d: %s"), PartyId, *MemberName);
        }

        // UI에 멤버가 나갔음을 알립니다.
        OnPartyMemberLeft.Broadcast(MemberName);
    }
 
}


/**
 * @brief 서버로부터 파티 호스트가 변경되었다는 알림을 받고 내부 상태를 업데이트합니다.
 * 이 함수는 "PartyHostTransferred" 패킷에 대응됩니다.
 * @param PartyId 업데이트가 발생한 파티 ID
 * @param OldHostName 이전 호스트의 이름
 * @param NewHostName 새로운 호스트의 이름
 */
void UGBPartyManager::HandlePartyHostTransferred(int32 PartyId, const FString& OldHostName, const FString& NewHostName)
{
    // 받은 PartyId가 현재 관리하는 파티와 일치하는지 확인합니다.
    //if (CurrentPartyInfo.PartyId == PartyId)
    //{
        // 현재 호스트 이름을 새로운 호스트 이름으로 업데이트합니다.
        CurrentPartyInfo.HostName = NewHostName;

        // UI에 호스트가 변경되었음을 알립니다.
        OnHostTransferred.Broadcast(PartyId, OldHostName, NewHostName);

        UpdateMyPartyRoleAndBroadCastIfChanged();

        UE_LOG(LogTemp, Warning, TEXT("Host transferred for Party %d. Old Host: %s, New Host: %s"),
            PartyId, *OldHostName, *NewHostName);
    //}
}

bool UGBPartyManager::IsInParty(const FString& UserId) const
{
    return CurrentPartyInfo.Members.Contains(UserId);
}

bool UGBPartyManager::IsPartyHost() const
{
    if (!CurrentPartyInfo.IsValid())
        return false;

    UGBLobbyUserManager* LobbyUserManager = GetGameInstance()->GetSubsystem<UGBLobbyUserManager>();


    if (LobbyUserManager)
    {
        if (LobbyUserManager->GetMyUserName() == CurrentPartyInfo.HostName)
            return true; 

        return false;
    }

    return false;
}

void UGBPartyManager::SendInviteToPartyRequest(const FString& TargetNickName)
{
    UE_LOG(LogTemp, Warning, TEXT("SendInviteToPartyRequest Called!"));

    // 만약 내 파티인원이 꽉 찼으면 보내지 않는다.
    // UI알림까지 해주면 좋지만, Popup시스템이 시원찮아서 일단 스킵.. 
    if (CurrentPartyInfo.IsFull())
    {
        UE_LOG(LogTemp, Warning, TEXT("[GBPartyManager.cpp] Not Sending Invite Request. Party Is Full"));
        return;
    }

    const FString MyNickName = GetWorld()->GetGameInstance()->GetSubsystem<UGBClientSessionSubsystem>()->GetNickname();
    GBClientSocketRequestHelper::SendPartyInviteRequest(MyNickName, TargetNickName);
}

void UGBPartyManager::SendKickFromPartyRequest(const FString& TargetNickName)
{
    UE_LOG(LogTemp, Warning, TEXT("Send KickFromPartyRequest Called!"));

    const FString MyNickName = GetWorld()->GetGameInstance()->GetSubsystem<UGBClientSessionSubsystem>()->GetNickname();
    GBClientSocketRequestHelper::SendKickFromPartyRequest(MyNickName, TargetNickName);

}

void UGBPartyManager::SendPartyHostTransferRequest(const FString& TargetNickName)
{
    UE_LOG(LogTemp, Warning, TEXT("Send Party Host Transfer Called!"));

    const FString MyNickName = GetWorld()->GetGameInstance()->GetSubsystem<UGBClientSessionSubsystem>()->GetNickname();
    GBClientSocketRequestHelper::SendPartyHostTransferRequest(MyNickName, TargetNickName);

}

void UGBPartyManager::SendLeaveFromPartyRequest(const FString& MyNickName)
{
    UE_LOG(LogTemp, Warning, TEXT("Send Leave From Party Request Called!"));

    GBClientSocketRequestHelper::SendLeaveFromPartyRequest(MyNickName);
}

void UGBPartyManager::AcceptCurrentInvite()
{
    if (PendingInvites.Num() == 0)
        return;

    const FPartyInvite Invite = PendingInvites[0];

    // 서버에 수락 요청 전송
    GBClientSocketRequestHelper::SendPartyInviteAcceptRequest(Invite.InviterName);

    // 큐 pop
    PendingInvites.RemoveAt(0);
    bInvitePopupActive = false;

    // 다음 초대 자동 표시
    TryPresentNextInvite();
}

void UGBPartyManager::DeclineCurrentInvite()
{
    if (PendingInvites.Num() == 0)
        return;

    const FPartyInvite Invite = PendingInvites[0];

    // 서버에 수락 거절 전송 해도되고 안해도되고. 서버가 굳이 알아야 할까?
    // GBClientSocketRequestHelper::SentPartyInviteDeclineRequest(?)

    // 큐 Pop
    PendingInvites.RemoveAt(0);
    bInvitePopupActive = false;

    // 다음 초대 자동 표시
    TryPresentNextInvite();
}

//void UGBFriendManager::SendRemoveFriendRequest(const FString& TargetNickName)
//{
//    UE_LOG(LogTemp, Warning, TEXT("SendRemoveFriendRequest Called!"));
//
//    const FString MyNickName = GetWorld()->GetGameInstance()->GetSubsystem<UGBClientSessionSubsystem>()->GetNickname();
//
//    GBClientSocketRequestHelper::SendRemoveFriendRequest(MyNickName, TargetNickName);
//}


