// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GBFriendManager.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FOnFriendRemoved, const FString& /*NickName*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnFriendAdded, const FString& /*NickName*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSentFriendRequestAdded, const FString& /*NickName*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSentFriendRequestRemoved, const FString& /*NickName*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnReceivedFriendRequestAdded, const FString& /*NickName*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnReceivedFriendRequestRemoved, const FString& /*NickName*/);

UCLASS()
class GREEDBOUND_API UGBFriendManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:
    // 초기화 
    // void InitFriendList(const TSet<FString>& InFriendNames);

    // 조회용
    const TSet<FString>& GetSentFrinedRequests() const;
    const TSet<FString>& GetReceivedFriendRequests() const;
    const TSet<FString>& GetFriendList() const;


    // 추가 삭제 
    void AddFriend(const FString& NickName);
    void RemoveFriend(const FString& NickName);
    void AddSentFriendRequest(const FString& NickName);
    void RemoveSentFriendRequest(const FString& NickName);
    void AddReceivedFriendRequest(const FString& NickName);
    void RemoveReceivedFriendRequest(const FString& Nickname); 


    // 델리게이트 등록
    FORCEINLINE FDelegateHandle BindOnFriendAdded(const FOnFriendAdded::FDelegate& Delegate) { return OnFriendAdded.Add(Delegate); }
    FORCEINLINE FDelegateHandle BindOnFriendRemoved(const FOnFriendRemoved::FDelegate& Delegate) { return OnFriendRemoved.Add(Delegate); }
    FORCEINLINE FDelegateHandle BindOnSentFriendRequestAdded(const FOnSentFriendRequestAdded::FDelegate& Delegate) { return OnSentFriendRequestAdded.Add(Delegate); }
    FORCEINLINE FDelegateHandle BindOnSentFriendRequestRemoved(const FOnSentFriendRequestRemoved::FDelegate& Delegate) { return OnSentFriendRequestRemoved.Add(Delegate); }
    FORCEINLINE FDelegateHandle BindOnReceivedFriendRequestAdded(const FOnReceivedFriendRequestAdded::FDelegate& Delegate) { return OnReceivedFriendRequestAdded.Add(Delegate); }
    FORCEINLINE FDelegateHandle BindOnReceivedFriendRequestRemoved(const FOnReceivedFriendRequestRemoved::FDelegate& Delegate) { return OnReceivedFriendRequestRemoved.Add(Delegate); }

    FORCEINLINE void UnbindOnFriendAdded(FDelegateHandle Handle) { OnFriendAdded.Remove(Handle); }
    FORCEINLINE void UnbindOnFriendRemoved(FDelegateHandle Handle) { OnFriendRemoved.Remove(Handle); }
    FORCEINLINE void UnbindOnSentFriendRequestAdded(FDelegateHandle Handle) { OnSentFriendRequestAdded.Remove(Handle); }
    FORCEINLINE void UnbindOnSentFriendRequestRemoved(FDelegateHandle Handle) { OnSentFriendRequestRemoved.Remove(Handle); }
    FORCEINLINE void UnbindOnReceivedFriendRequestAdded(FDelegateHandle Handle) { OnReceivedFriendRequestAdded.Remove(Handle); }
    FORCEINLINE void UnbindOnReceivedFriendRequestRemoved(FDelegateHandle Handle) { OnReceivedFriendRequestRemoved.Remove(Handle); }


    // 로비에서 퇴장 시 처리
    void HandleUserRemoved(const FString& NickName);

    // 특정 유저가 친구인지 확인
    bool IsFriend(const FString& NickName) const;


    // 백엔드 서버에 친구 추가 요청을 보냄
    void SendAddFriendRequest(const FString& TargetNickName);

    // 백엔드 서버에 친구 제거 요청을 보냄
    void SendRemoveFriendRequest(const FString& TargetNickName);

    // 백엔드 서버에 친구 추가 요청을 철회함을 보냄
    void SendWithdrawFriendRequest(const FString& TargetNickName);

    // 백엔드 서버에 받은 친구추가 요청 승락/거부를 보냄
    void SendRespondToFriendRequest(const FString& TargetNickName, bool bAccepted);
    

private :
    // UI 등에서 사용
    FOnFriendAdded OnFriendAdded;
    FOnFriendRemoved OnFriendRemoved;
    FOnSentFriendRequestAdded OnSentFriendRequestAdded;
    FOnSentFriendRequestRemoved OnSentFriendRequestRemoved;
    FOnReceivedFriendRequestAdded OnReceivedFriendRequestAdded;
    FOnReceivedFriendRequestRemoved OnReceivedFriendRequestRemoved;

private:
    UPROPERTY()
    TSet<FString> FriendNameSet;

    UPROPERTY()
    TSet<FString> SentFriendRequests;

    UPROPERTY()
    TSet<FString> ReceivedFriendRequests;
};
