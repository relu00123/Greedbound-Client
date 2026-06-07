// Fill out your copyright notice in the Description page of Project Settings.


#include "GB/Subsystem/Network/Dedicated/GBDedicatedSocketRequestHelper.h"
#include "GBDedicatedSocketSubsystem.h"
#include "JsonObjectConverter.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Define/GBDefine.h"

void UGBDedicatedSocketRequestHelper::SendChangeClientGamePhase(const FString& ChangedClientToken, EGBClientGamePhase GamePhase)
{
    GB_BACKEND_LOG(Warning, TEXT("Send Change Client "));

    TSharedPtr<FJsonObject> Payload = MakeShared<FJsonObject>();

    Payload->SetStringField(TEXT("ClientToken"), ChangedClientToken);
    const UEnum* Enum = StaticEnum<EGBClientGamePhase>();
    FString NewPhaseAsString =  Enum->GetNameStringByValue((int64)GamePhase);
    Payload->SetStringField(TEXT("NewPhase"), NewPhaseAsString);

    SendJsonRequest(TEXT("ChangeClientGamePhaseRequest"), Payload);
}

void UGBDedicatedSocketRequestHelper::SendJsonRequest(const FString& Type, const TSharedPtr<FJsonObject>& Payload)
{
    UWorld* World = GEngine->GetWorldFromContextObjectChecked(UGameplayStatics::GetGameInstance(GEngine->GetWorldContexts()[0].World()));
    if (!World)
    {
        return;
    }

    UGameInstance* GameInstance = World->GetGameInstance();
    if (!GameInstance) 
    { 
        return;
    }

    UGBDedicatedSocketSubsystem* SocketSubsystem = GameInstance->GetSubsystem<UGBDedicatedSocketSubsystem>();
    if (!SocketSubsystem || !SocketSubsystem->IsConnected())
    {
        GB_BACKEND_LOG(Error, TEXT("Socket is not connected."));
        return;
    }

    // 메시지 본체 구성
    TSharedPtr<FJsonObject> RootJson = MakeShared<FJsonObject>();
    RootJson->SetStringField(TEXT("type"), Type);
    RootJson->SetObjectField(TEXT("Payload"), Payload ? Payload : MakeShared<FJsonObject>());

    // 직렬화
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    if (FJsonSerializer::Serialize(RootJson.ToSharedRef(), Writer))
    {
        SocketSubsystem->SendJsonString(OutputString);
        GB_BACKEND_LOG(Log, TEXT("Sent JSON Request: %s"), *OutputString);
    }
    else
    {
        GB_BACKEND_LOG(Error, TEXT("Failed to serialize JSON request."));
    }
}

