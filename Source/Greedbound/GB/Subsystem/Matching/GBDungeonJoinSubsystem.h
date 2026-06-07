// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Define/Match/GBDungeonJoinTypes.h"
#include "GBDungeonJoinSubsystem.generated.h"

// 진행 상태
UENUM(BlueprintType)
enum class EDungeonJoinState : uint8
{
    Idle,
    Joining
};


UCLASS()
class GREEDBOUND_API UGBDungeonJoinSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    // Subsystem
    virtual void                    Deinitialize() override;

    // 접속 시작(소켓 핸들러에서 호출)
    UFUNCTION(BlueprintCallable)
    void                            ConnectToDungeon(const FDungeonJoinInfo& Info, float TimeoutSec = 20.f);

    // 취소(선택적)
    UFUNCTION(BlueprintCallable)
    bool                            CancelJoin(const FString& Reason = TEXT("USER_CANCELED"));

    UFUNCTION(BlueprintPure)
    bool                            IsJoining() const { return State == EDungeonJoinState::Joining; }


    // Delegates
    FORCEINLINE FDelegateHandle     BindOnJoinStarted(const FDungeonJoinStarted::FDelegate& Delegate) { return OnJoinStarted.Add(Delegate); }
    FORCEINLINE void                UnbindOnJoinStarted(FDelegateHandle Handle) { OnJoinStarted.Remove(Handle); }
 
private:
    // 내부
    FString                         BuildTravelURL(const FDungeonJoinInfo& Info) const;
    void                            BindFailureDelegates();
    void                            UnbindFailureDelegates();
    void                            OnJoinTimeout();

    // 엔진 콜백
    void                            HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
    void                            HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString);
    void                            HandlePostLoadMap(UWorld* LoadedWorld);

    // 종료 루틴
    void                            FinishSuccess();
    void                            FinishFailure(const FString& Reason, int32 Code = 0);


private:
    UPROPERTY() 
    EDungeonJoinState               State = EDungeonJoinState::Idle;

    UPROPERTY() 
    FDungeonJoinInfo                LastInfo;

    FTimerHandle                    JoinTimeoutHandle;

    // Delegate handles
    FDelegateHandle                 NetFailHandle;
    FDelegateHandle                 TravelFailHandle;
    FDelegateHandle                 PostLoadHandle;

    // 이벤트
    FDungeonJoinStarted             OnJoinStarted;
    FDungeonJoinSucceeded           OnJoinSucceeded;
    FDungeonJoinFailed              OnJoinFailed;
};
