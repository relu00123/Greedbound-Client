// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GBClientSocketSubsystem.generated.h"

class IWebSocket;
class FJsonObject;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSocketConnected);


UCLASS()
class GREEDBOUND_API UGBClientSocketSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    void                        ConnectSocket(const FString& Token, const FString& NickName);
    bool                        IsConnected() const;

    void                        RegisterHandler(const FString& MessageType, TFunction<void(const TSharedPtr<FJsonObject>&)> Handler);

    void                        ConnectToDedicatedServer(const FString& Host, int32 Port);

    void                        SendJsonString(const FString& Message);

    UPROPERTY(BlueprintAssignable)
    FOnSocketConnected          OnSocketConnected;

protected:
    virtual void                Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void                Deinitialize() override;

private:
    void                        HandleMessage(const FString& Message);

private:
    TSharedPtr<IWebSocket>      Socket;
    TMap<FString, TFunction<void(const TSharedPtr<FJsonObject>&)>> MessageHandlers;
};
