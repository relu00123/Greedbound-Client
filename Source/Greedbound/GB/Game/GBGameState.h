// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GBGameState.generated.h"

UENUM(BlueprintType)
enum class EGBSessionMode : uint8
{ 
    Unknown             UMETA(DisplayName="Unknown"),
    LocalTest           UMETA(DisplayName="LocalTest"),             // PIE, StandAlone, Listen, Play as Cliente
    DedicatedServer     UMETA(DisplayName="DedicatedServer"),       // BackEndServer에서 띄운 DedicatedServer
};

UCLASS()
class GREEDBOUND_API AGBGameState : public AGameState
{
    GENERATED_BODY()
    
public:
    AGBGameState();

    FORCEINLINE EGBSessionMode  GetSessionMode() const { return SessionMode; }
    FORCEINLINE void            SetSessionMode(EGBSessionMode InMode) { SessionMode = InMode; }

protected :
    virtual void                GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(Replicated)
    EGBSessionMode              SessionMode;
};
