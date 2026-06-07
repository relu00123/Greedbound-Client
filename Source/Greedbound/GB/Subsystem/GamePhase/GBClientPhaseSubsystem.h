// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Define/GamePhase/GBClientGamePhase.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Define/Match/GBDungeonJoinTypes.h"
#include "GBClientPhaseSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class GREEDBOUND_API UGBClientPhaseSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
protected:
    virtual void                            Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void                            Deinitialize() override;
    
public :
    // 소캣 으로부터 패킷 수신시 
    UFUNCTION()
    void                                    HandleGamePhaseChanged(bool bSuccess, EGBClientGamePhase beforeState, EGBClientGamePhase ChangedState);

    // Delegates
    FORCEINLINE FDelegateHandle             BindOnClientGamePhaseChanged(const FOnMyGamePhaseChanged::FDelegate& Delegate) { return OnMyGamePhaseChanged.Add(Delegate); }
    FORCEINLINE void                        UnbindOnClientGamePhaseChanged(FDelegateHandle Handle) { OnMyGamePhaseChanged.Remove(Handle); }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE EGBClientGamePhase          GetMyGamePhase() { return MyGamePhase;}
    
    UFUNCTION(BlueprintCallable)
    void                                    SetMyGamePhase(EGBClientGamePhase BeforePhase,  EGBClientGamePhase NewPhase);

    // 클라 -> 서버에 GamePhase를 변경해달라고 요청 
    void                                    SendGamePhaseChangeFromClient(EGBClientGamePhase NewPhase); 

    static FString                          GamePhaseToString(EGBClientGamePhase GamePhase);
    static bool                             StringToGamePhase(const FString& Str, EGBClientGamePhase& OutPhase);

private:
    void                                    HandleEtner(EGBClientGamePhase NewPhase);
    void                                    HandleExit(EGBClientGamePhase OldPhase);

    void                                    HandleOnDungeonJoinStarted(const FDungeonJoinInfo& DungeonInfo);

private:
    // Delegates 
    FOnMyGamePhaseChanged                   OnMyGamePhaseChanged;

    FDelegateHandle                         OnDungeonJoinStartedHandle;

    EGBClientGamePhase                      MyGamePhase = EGBClientGamePhase::None;
};
