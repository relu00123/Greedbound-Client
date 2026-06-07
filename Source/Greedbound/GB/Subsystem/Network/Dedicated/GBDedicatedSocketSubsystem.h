// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GBDedicatedSocketSubsystem.generated.h"

class IWebSocket;

UCLASS()
class GREEDBOUND_API UGBDedicatedSocketSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
protected :
    virtual void                        Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void                        Deinitialize() override;

public:
    void                                ConnectSocket(const FString& Token);
    void                                DisconnectSocket();

    bool                                IsConnected() const;
    FORCEINLINE TSharedPtr<IWebSocket>  GetSocket() const { return Socket; }

    void                                SendJsonString(const FString& Message);

    // Send
    void                                SendEscapeRequest(const struct FEscapePayload& Payload);

private:
    void                                HandleMessage(const FString& Message);
    void                                SendJsonMessage(const FString& Type, const TSharedPtr<FJsonObject>& Payload);

private:
    TSharedPtr<IWebSocket>              Socket;
};
