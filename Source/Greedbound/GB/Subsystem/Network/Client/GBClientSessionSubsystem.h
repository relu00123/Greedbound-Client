// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GBClientSessionSubsystem.generated.h"

//UENUM(BlueprintType)
//enum class EPlayerState : uint8
//{
//    IDLE    UMETA(DisplayName = "Idle"),
//    READY    UMETA(DisplayName = "Ready"),
//    INGAME    UMETA(DisplayName = "InGame"),
//    DEAD    UMETA(DisplayName = "Dead")
//};

UCLASS()
class GREEDBOUND_API UGBClientSessionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:
    // 로그인 후 세션 정보 설정
    void            SetAuthToken(const FString& InToken);
    void            SetNickName(const FString& InNickname);
    // void SetPlayerState(EPlayerState InState);

    // 서버에게 로그아웃 보내기
    void            ClearSession();
    void            SendLogoutToServer();

    // Getter
    const FString&    GetAuthToken() const { return AuthToken; }
    const FString&    GetNickname() const { return Nickname; }
    // EPlayerState GetPlayerState() const { return PlayerState; }

    // 로그인 상태 확인
    bool            IsLoggedIn() const { return !AuthToken.IsEmpty(); }

private:
    FString            AuthToken;
    FString            Nickname;
    //EPlayerState PlayerState = EPlayerState::IDLE;
};
