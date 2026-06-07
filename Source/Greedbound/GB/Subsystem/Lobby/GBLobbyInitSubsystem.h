// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GBLobbyInitSubsystem.generated.h"

class UGBPartyManager;
class UGBLobbyUserManager;

UENUM()
enum class ELobbyInitStep : uint8
{
    None                        = 0,
    PlayerList                  = 1 << 0,
    FriendList                  = 1 << 1,
    FriendRequestSentList       = 1 << 2,
    FriendRequestReceivedList   = 1 << 3,
    ALL = PlayerList | FriendList | FriendRequestSentList | FriendRequestReceivedList  
};

ENUM_CLASS_FLAGS(ELobbyInitStep)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbyInitialized);

// 로그인 이후 로비 진입 전 필요한 초기화 상태들을 추적하는 Subsystem 
UCLASS()
class GREEDBOUND_API UGBLobbyInitSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public :
    // 모든 초기화가 완료되었을시 호출되는 델리게이트 
    UPROPERTY(BlueprintAssignable)
    FOnLobbyInitialized OnLobbyInitialized;     // 지금 사용되는 곳은 없는듯? 

protected:
    virtual void        Initialize(FSubsystemCollectionBase& Collection) override;

public:
    // 초기화 해야하는 대상 설정 - 보통은 All
    void                SetTargetInitSteps(ELobbyInitStep TargetSteps); 

    // 특정 항목 초기화 완료 시 호출
    void                MarkStepCompleted(ELobbyInitStep Step);

    // 모든 초기화가 완료되었는가?
    bool                IsInitializationComplete() const { return RemainingSteps == ELobbyInitStep::None; }

    UFUNCTION()
    void                HandleAllInitCompleted();

private:
    ELobbyInitStep      RemainingSteps = ELobbyInitStep::ALL;
};
