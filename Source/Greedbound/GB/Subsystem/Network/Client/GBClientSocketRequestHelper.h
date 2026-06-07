// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Define/User/UserInfo.h"

class FJsonObject;

class GREEDBOUND_API GBClientSocketRequestHelper
{
public:
    // 목록 요청 함수들

    // 클래스 변경 요청
    static void SendChangeClassRequest(EGBCharacterClassType RequestedClass);
    
    // 내 정보 변경 요청
    static void SendChangeMyUserInfo(const FUserInfo& MyUserInfo);    
    
    // 로비 유저 목록 요청
    static void SendLobbyUserListRequest();

    // 친구 목록 요청
    static void SendFriendListRequest(const FString& MyNickName);

    // 보낸 친구추가 목록 요청
    static void SendFriendRequestSentListRequest(const FString& MyNickName);

    // 받은 친구추가 목록 요청
    static void SendFriendRequestReceivedListRequest(const FString& MyNickName);


    // 친구 추가 요청
    static void SendAddFriendRequest(const FString& MyNickName, const FString& TargetNickname);

    // 친구 제거 요청
    static void SendRemoveFriendRequest(const FString& MyNickName, const FString& TargetNickname);

    // 친구 추가 요청 취소
    static void SendWithdrawFriendRequest(const FString& MyNickName, const FString& TargetNickname);

    // 친구 추가 요청 응답
    static void SendRespondToFriendRequest(const FString& MyNickName, const FString& TargetNickname, bool isAccepted);


    // 파티 초대 요청
    static void SendPartyInviteRequest(const FString& MyNickName, const FString& TargetNickname);

    // 파티장 변경 요청
    static void SendPartyHostTransferRequest(const FString& HostNickName, const FString& TargetNickName);

    // 파티 초대 응답
    static void SendPartyInviteAcceptRequest(const FString& InviterName);

    // 파티에서 추방 요청
    static void SendKickFromPartyRequest(const FString& HostNickName, const FString& TargetNickName);

    // 파티 탈퇴 요청
    static void SendLeaveFromPartyRequest(const FString& MyNickName);


    // GamePhase 변경 요청 (클라 -> 서버로의 요청, 서버는 받자마자 처리후 클라에게 변경완료를 알림) 
    static void SendGamePhaseChangeRequest(const FString& NewPhase);


    // 매치 시작 요청
    static void SendMatchStartRequest(int32 MapNumericID);

    // 매치 취소 요청
    static void SendMatchCancelRequest(int32 MapNumericID);



private:
    static void SendJsonRequest(const FString& Type, const TSharedPtr<FJsonObject>& Payload = nullptr);
};
