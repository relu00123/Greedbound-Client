// Fill out your copyright notice in the Description page of Project Settings.


#include "GB/Subsystem/Player/GBPlayerStatusSubsystem.h"

FProfileContext UGBPlayerStatusSubsystem::MakeProfileContextFor(const FString& PlayerNickName) const
{
    FProfileContext Context;
    Context.TargetNickName = PlayerNickName;
    Context.bIsFriend = FriendList.Contains(PlayerNickName);
    Context.bIsInParty = PartyList.Contains(PlayerNickName);
    Context.bIsBlocked = BlockedList.Contains(PlayerNickName);
    return Context;
}

void UGBPlayerStatusSubsystem::RegisterUserInfo(const FUserInfo& Info)
{
    AllPlayers.Add(Info.NickName, Info);
}

void UGBPlayerStatusSubsystem::RegisterFriend(const FString& PlayerNickName)
{
    FriendList.Add(PlayerNickName);
}

void UGBPlayerStatusSubsystem::RegisterPartyMember(const FString& PlayerNickName)
{
    PartyList.Add(PlayerNickName);
}

void UGBPlayerStatusSubsystem::RegisterBlockedPlayer(const FString& PlayerNickName)
{
    BlockedList.Add(PlayerNickName);
}

const FUserInfo* UGBPlayerStatusSubsystem::GetUserInfo(const FString& PlayerNickName) const
{
    return AllPlayers.Find(PlayerNickName);
}

bool UGBPlayerStatusSubsystem::IsFriend(const FString& PlayerNickName) const
{
    return FriendList.Contains(PlayerNickName);
}

 

bool UGBPlayerStatusSubsystem::IsBlocked(const FString& PlayerNickName) const
{
    return BlockedList.Contains(PlayerNickName);
}
