// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Define/Match/GBMatchInfo.h"
#include "Game/GBGameMode.h"
#include "GBDungeonGameMode.generated.h"

class AGBDungeonGameState;
class AGBCharacterSpawnPoint;
class UGBClassPresentationData;
class AGBPlayerState;
struct FGBClassPresentation;
class AGBPlayerController;

// 팀 예약 레코드
USTRUCT()
struct FTeamSpawnPointReservation
{
    GENERATED_BODY()
    TWeakObjectPtr<AGBCharacterSpawnPoint> Anchor;
    TArray<FTransform> Slots;
    int32 NextIndex = 0;
};

UCLASS()
class GREEDBOUND_API AGBDungeonGameMode : public AGBGameMode
{
    GENERATED_BODY()

public:
    AGBDungeonGameMode();

    FORCEINLINE void                                SetAssignedMatchInfo(const FGBMatchInfo& InMatchInfo) { AssignedMatchInfo = InMatchInfo; }
    FORCEINLINE const FGBMatchInfo&                 GetAssignedMatchInfo() { return AssignedMatchInfo; }

    void                                            InitFromAssignedMatchInfo();
    void                                            HandleClientLoadedDungeonMap(AGBPlayerController* PlayerController);
    void                                            HandleDungeonMapLoadedCountChanged();

protected:
    virtual void                                    StartPlay() override;
    virtual void                                    BeginPlay() override;

    // 접속 직전(토큰/옵션 확인)
    virtual void                                    PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

    // 컨트롤러 생성 후(카운트/팀 반영)
    virtual FString                                 InitNewPlayer(APlayerController* NewPC, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;

    virtual void                                    PostLogin(APlayerController* NewPlayer) override;

    // 초기 스폰 : 클래스별 Pawn 선택
    virtual UClass*                                 GetDefaultPawnClassForController_Implementation(AController* InController) override;

    virtual void                                    Logout(AController* Exiting) override;
    virtual void                                    RestartPlayer(AController* NewPlayer) override;

private:
    int32                                           GetActivePlayerCount() const;
    void                                            RequestServerShutdown() const;

    // Util
    int32                                           GetExpectedPlayerCount() const;
    int32                                           GetConnectedPlayerCount() const;
    AGBDungeonGameState*                            GS() const;

    void                                            ArmCountdownForGameStart(float Seconds);
    void                                            CancelCountdownForGameStart();

    AGBCharacterSpawnPoint*                         ReserveAnchorForTeam(const FString& TeamId, int32 TeamSize);
    bool                                            PopNextSlotForTeam(const FString& TeamId, FTransform& Out);

    bool                                            GetClassPresentationFromPS(const AGBPlayerState* PS, FGBClassPresentation& Out) const;
    TSubclassOf<APawn>                              ChoosePawnFromPS(const AGBPlayerState* PS) const;
    TSubclassOf<AHUD>                               ChooseHUDFromPS(const AGBPlayerState* PS) const;

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Class|Presentation", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UGBClassPresentationData>            ClassPresentationData;

    UPROPERTY()
    TMap<FString, FTeamSpawnPointReservation>       TeamSpawnPointReservations;

    TArray<TWeakObjectPtr<AGBCharacterSpawnPoint>>  CharacterSpawnPool;

    FGBMatchInfo                                    AssignedMatchInfo;

    TMap<TWeakObjectPtr<AController>, FString>      TeamIdByController;

    TMap<FString, int32>                            ExpectedPlayerCountByTeam;
    TMap<FString, int32>                            ConnectedPlayerCountByTeam;

    FTimerHandle                                    MatchStartCountdownTickHandle;

    UPROPERTY()
    TArray<TWeakObjectPtr<APlayerController>>       ReadyPlayers;
};
