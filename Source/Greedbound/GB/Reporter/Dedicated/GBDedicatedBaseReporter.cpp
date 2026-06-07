// Fill out your copyright notice in the Description page of Project Settings.

#include "GBDedicatedBaseReporter.h"
#include "Subsystem/Network/Dedicated/GBDedicatedSocketSubsystem.h"
#include "Engine/GameInstance.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "IWebSocket.h"

void UGBDedicatedBaseReporter::Init(UGameInstance* GameInstance)
{
    SocketSubsystem = GameInstance->GetSubsystem<UGBDedicatedSocketSubsystem>();

    if (!SocketSubsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("[BaseReporter] SocketSubsystem not found!"));
    }
}

void UGBDedicatedBaseReporter::SendJson(const TSharedPtr<FJsonObject>& Json)
{
    if (!SocketSubsystem || !SocketSubsystem->IsConnected())
    {
        UE_LOG(LogTemp, Warning, TEXT("[BaseReporter] WebSocket is not connected."));
        return;
    }

    FString Output;
    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
    if (FJsonSerializer::Serialize(Json.ToSharedRef(), Writer))
    {
        SocketSubsystem->GetSocket()->Send(Output);
        UE_LOG(LogTemp, Log, TEXT("[BaseReporter] Sent JSON: %s"), *Output);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[BaseReporter] Failed to serialize JSON."));
    }
}
