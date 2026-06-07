// Fill out your copyright notice in the Description page of Project Settings.

#include "GBClientSessionSubsystem.h"
#include "GB/Subsystem/Network/Client/GBClientHttpManagerSubsystem.h"

void UGBClientSessionSubsystem::SetAuthToken(const FString& InToken)
{
    AuthToken = InToken;
    UE_LOG(LogTemp, Log, TEXT("[Session] 토큰 저장됨"));
}

void UGBClientSessionSubsystem::SetNickName(const FString& InNickname)
{
    Nickname = InNickname;
}

void UGBClientSessionSubsystem::ClearSession()
{
    AuthToken.Empty();
    Nickname.Empty();
    //PlayerState = EPlayerState::IDLE;

    UE_LOG(LogTemp, Log, TEXT("세션 초기화됨"));
}

void UGBClientSessionSubsystem::SendLogoutToServer()
{
    if (AuthToken.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("로그아웃 요청 실패 : 저장된 AuthToken이 없습니다."));
        return;
    }

    if (UGBClientHttpManagerSubsystem* Http = GetGameInstance()->GetSubsystem<UGBClientHttpManagerSubsystem>())
    {
        Http->SendLogoutRequest(AuthToken);
    }
}

//void UGBSessionSubsystem::SetPlayerState(EPlayerState InState)
//{
//    PlayerState = InState;
//}

//void UGBSessionSubsystem::ClearSession()
//{
//    AuthToken.Empty();
//    Nickname.Empty();
//    //PlayerState = EPlayerState::IDLE;
//
//    UE_LOG(LogTemp, Log, TEXT("[Session] 세션 초기화됨"));
//}
