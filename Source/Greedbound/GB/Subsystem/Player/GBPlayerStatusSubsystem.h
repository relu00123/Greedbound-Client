// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Define/Character/GBClassType.h"
#include "Define/User/UserInfo.h"
#include "GBPlayerStatusSubsystem.generated.h"

 
// 클라이언트에서 친구/파티/차단 등 상태를 저장하는 Subsystem
UCLASS()
class GREEDBOUND_API UGBPlayerStatusSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:
    // 친구/파티/차단 여부를 포함한 ProfileContext 반환
    FProfileContext MakeProfileContextFor(const FString& playerNickName) const; 

    // 전체 플레이어 정보 등록
    void            RegisterUserInfo(const FUserInfo& Info); 

    /** 친구/파티/차단 목록 등록 */
    void            RegisterFriend(const FString& PlayerNickName); 
    void            RegisterPartyMember(const FString& PlayerNickName);
    void            RegisterBlockedPlayer(const FString& PlayerNickName);

    /** 접근 함수들 */
    const FUserInfo* GetUserInfo(const FString& PlayerNickName) const;
    bool            IsFriend(const FString& PlayerNickName) const;
    bool            IsInParty(const FString& PlayerNickName) const;
    bool            IsBlocked(const FString& PlayerNickName) const;

private:
    UPROPERTY()
    TMap<FString, FUserInfo> AllPlayers;

    UPROPERTY()
    TSet<FString>   FriendList;

    UPROPERTY()
    TSet<FString>   PartyList;

    UPROPERTY()
    TSet<FString>   BlockedList;
};
