// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Game/GBGameState.h"
#include "GBDungeonGameState.generated.h"

 
UCLASS()
class GREEDBOUND_API AGBDungeonGameState : public AGBGameState
{
    GENERATED_BODY()
    
public:
    void                        AddDungeonMapLoadedPlayer();

    FORCEINLINE int32           GetDungeonMapLoadedPlayerCount() const { return DungeonMapLoadedPlayerCount; }

    FORCEINLINE bool            GetMatchStarted() const { return bMatchStarted; }
    FORCEINLINE void            SetMatchStarted(bool bState) { bMatchStarted = bState; }

    UFUNCTION()
    void                        OnRep_MatchStarted();

private :
    UFUNCTION()
    void                        OnRep_DungeonMapLoadedPlayerCount();
  
    
public:
    // 전체 인원 집계
    UPROPERTY(Replicated, BlueprintReadOnly) 
    int32                       ExpectedPlayers = 0;

    UPROPERTY(Replicated, BlueprintReadOnly) 
    int32                       ConnectedPlayers = 0;

    UPROPERTY(Replicated, BlueprintReadOnly) 
    float CountdownRemaining = 0.f;

    virtual void                GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


private:
    UPROPERTY(ReplicatedUsing = OnRep_DungeonMapLoadedPlayerCount, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    int32                       DungeonMapLoadedPlayerCount = 0;

    UPROPERTY(ReplicatedUsing = OnRep_MatchStarted, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    bool                        bMatchStarted = false;   

};
