// Fill out your copyright notice in the Description page of Project Settings.


#include "GBFriendManager.h"
#include "Subsystem/Network/Client/GBClientSocketRequestHelper.h"
#include "Subsystem/Network/Client/GBClientSessionSubsystem.h"


//void UGBFriendManager::InitFriendList(const TSet<FString>& InFriendNames)
//{
//    FriendNameSet = InFriendNames;
//}

void UGBFriendManager::AddFriend(const FString& NickName)
{
    if (FriendNameSet.Contains(NickName))
        return;

    FriendNameSet.Add(NickName);
    OnFriendAdded.Broadcast(NickName);
}

void UGBFriendManager::RemoveFriend(const FString& NickName)
{
    if (!FriendNameSet.Contains(NickName))
        return;

    FriendNameSet.Remove(NickName);
    OnFriendRemoved.Broadcast(NickName);
}

void UGBFriendManager::AddSentFriendRequest(const FString& NickName)
{
    if (SentFriendRequests.Contains(NickName))
        return;

    SentFriendRequests.Add(NickName);
    OnSentFriendRequestAdded.Broadcast(NickName);
}

void UGBFriendManager::RemoveSentFriendRequest(const FString& NickName)
{
    if (!SentFriendRequests.Contains(NickName))
        return;

    SentFriendRequests.Remove(NickName);
    OnSentFriendRequestRemoved.Broadcast(NickName);
}

void UGBFriendManager::AddReceivedFriendRequest(const FString& NickName)
{
    if (ReceivedFriendRequests.Contains(NickName))
        return;

    ReceivedFriendRequests.Add(NickName);
    OnReceivedFriendRequestAdded.Broadcast(NickName);
}

void UGBFriendManager::RemoveReceivedFriendRequest(const FString& Nickname)
{
    if (!ReceivedFriendRequests.Contains(Nickname))
        return;

    ReceivedFriendRequests.Remove(Nickname);
    OnReceivedFriendRequestRemoved.Broadcast(Nickname);
}

void UGBFriendManager::HandleUserRemoved(const FString& NickName)
{
    // 유저가 로비를 나갔을 때 친구 목록에서도 제거
    if (FriendNameSet.Contains(NickName))
    {
        RemoveFriend(NickName);
    }
}

bool UGBFriendManager::IsFriend(const FString& NickName) const
{
    return FriendNameSet.Contains(NickName);
}

const TSet<FString>& UGBFriendManager::GetSentFrinedRequests() const
{
    return SentFriendRequests;
}

const TSet<FString>& UGBFriendManager::GetReceivedFriendRequests() const
{
    return ReceivedFriendRequests;
}

const TSet<FString>& UGBFriendManager::GetFriendList() const
{
    return FriendNameSet;
}

void UGBFriendManager::SendAddFriendRequest(const FString& TargetNickName)
{
    UE_LOG(LogTemp, Warning, TEXT("SendAddFriendRequest Called!"));

    const FString MyNickName = GetWorld()->GetGameInstance()->GetSubsystem<UGBClientSessionSubsystem>()->GetNickname();

    // 1. Client는 BackEndServer로 친구 추가 요청을 보냄
    GBClientSocketRequestHelper::SendAddFriendRequest(MyNickName, TargetNickName);
}

void UGBFriendManager::SendRemoveFriendRequest(const FString& TargetNickName)
{
    UE_LOG(LogTemp, Warning, TEXT("SendRemoveFriendRequest Called!"));

    const FString MyNickName = GetWorld()->GetGameInstance()->GetSubsystem<UGBClientSessionSubsystem>()->GetNickname();

    GBClientSocketRequestHelper::SendRemoveFriendRequest(MyNickName, TargetNickName);
}

void UGBFriendManager::SendWithdrawFriendRequest(const FString& TargetNickName)
{
    UE_LOG(LogTemp, Warning, TEXT("SendWithdrawFriendRequest Called!"));

    const FString MyNickName = GetWorld()->GetGameInstance()->GetSubsystem<UGBClientSessionSubsystem>()->GetNickname();

}

void UGBFriendManager::SendRespondToFriendRequest(const FString& TargetNickName, bool bAccepted)
{
    UE_LOG(LogTemp, Warning, TEXT("SendRespontToFriendRequest Called!"));

    const FString MyNickName = GetWorld()->GetGameInstance()->GetSubsystem<UGBClientSessionSubsystem>()->GetNickname();

    // 1. 응답을 했으면 해당 유저를 받은 친구 목록에서 삭제하도록 함.
    OnSentFriendRequestRemoved.Broadcast(TargetNickName);

    // 2. 서버에 응답 결과를 알림
    GBClientSocketRequestHelper::SendRespondToFriendRequest(MyNickName, TargetNickName, bAccepted);

}



