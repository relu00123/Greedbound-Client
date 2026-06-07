// Fill out your copyright notice in the Description page of Project Settings.


#include "GBDedicatedSocketSubsystem.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"
#include "Modules/ModuleManager.h" 
#include "Utils/GBJsonUtils.h"
#include "Network/Payload/FEscapePayload.h"
#include "Define/GBDefine.h"

void UGBDedicatedSocketSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
    {
        FModuleManager::LoadModuleChecked<FWebSocketsModule>("WebSockets");
    }
}

void UGBDedicatedSocketSubsystem::Deinitialize()
{
    if (Socket.IsValid() && Socket->IsConnected())
    {
        Socket->Close();
    }

    Socket = nullptr;
}

bool UGBDedicatedSocketSubsystem::IsConnected() const
{
    return Socket.IsValid() && Socket->IsConnected();
}

// DedicatedServer가 BackEndServer와 Socket을 연결하려 하는 부분. 
// BS의 setupSocket(wss : WebSocketServer)를 참고할 것. 
void UGBDedicatedSocketSubsystem::ConnectSocket(const FString& Token)
{
    const FString Url = FString::Printf(TEXT("ws://localhost:3000//ws?token=%s&connecttype=dedicated"), *Token);

    Socket = FWebSocketsModule::Get().CreateWebSocket(Url);

    Socket->OnConnected().AddLambda([]() {
        GB_BACKEND_LOG(Warning, TEXT("[DedicatedSocketSubsystem] WebSocket connected"));
        });

    Socket->OnConnectionError().AddLambda([](const FString& Error) {
        GB_BACKEND_LOG(Warning, TEXT("[DedicatedSocketSubsystem] WebSocket error: %s"), *Error);
        });

    Socket->OnClosed().AddLambda([](int32 Code, const FString& Reason, bool bWasClean) {
        GB_BACKEND_LOG(Warning, TEXT("[DedicatedSocketSubsystem] WebSocket closed: %s"), *Reason);
        });

    Socket->OnMessage().AddUObject(this, &UGBDedicatedSocketSubsystem::HandleMessage);
    Socket->Connect();
}

void UGBDedicatedSocketSubsystem::DisconnectSocket()
{
    if (Socket.IsValid())
    {
        if (Socket->IsConnected())
        {
            GB_BACKEND_LOG(Warning, TEXT("[DedicatedSocketSubsystem] Closing socket connection..."));
            Socket->Close(1000, TEXT("DS shutting down")); // 정상 종료 코드 (1000: normal closure)
        }
        else
        {
            GB_BACKEND_LOG(Warning, TEXT("[DedicatedSocketSubsystem] DisconnectSocket called but socket already closed."));
        }

        // 강제로 null 처리해서 메모리/상태 초기화
        Socket.Reset();
    }
    else
    {
        GB_BACKEND_LOG(Warning, TEXT("[DedicatedSocketSubsystem] DisconnectSocket called but socket was invalid."));
    }
}

void UGBDedicatedSocketSubsystem::HandleMessage(const FString& Message)
{
    GB_BACKEND_LOG(Warning, TEXT("[DedicatedSocketSubsystem] Received message: %s"), *Message);
    // 여기서 JSON 파싱 후 메시지 타입별 처리 가능
}

void UGBDedicatedSocketSubsystem::SendJsonMessage(const FString& Type, const TSharedPtr<FJsonObject>& Payload)
{

    if (!IsConnected())
    {
        GB_BACKEND_LOG(Warning, TEXT("[SocketSubsystem] Not connected"));
        return;
    }

    TSharedPtr<FJsonObject> Wrapper = MakeShared<FJsonObject>();
    Wrapper->SetStringField(TEXT("type"), Type);
    Wrapper->SetObjectField(TEXT("payload"), Payload);

    FString Output;
    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
    if (FJsonSerializer::Serialize(Wrapper.ToSharedRef(), Writer))
    {
        GetSocket()->Send(Output);
        GB_BACKEND_LOG(Warning, TEXT("[SocketSubsystem] Sent: %s"), *Output);
    }
    else
    {
        GB_BACKEND_LOG(Warning, TEXT("[SocketSubsystem] Failed to serialize JSON"));
    }
}

void UGBDedicatedSocketSubsystem::SendJsonString(const FString& Message)
{
    if (Socket.IsValid() && Socket->IsConnected())
    {
        Socket->Send(Message);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Attempted to send message but socket is not connected."));
    }

}

// 여기 위치 마음에 안드는데.. 
void UGBDedicatedSocketSubsystem::SendEscapeRequest(const FEscapePayload& Payload)
{
    TSharedPtr<FJsonObject> JsonPayload = FGBJsonUtils::StructToJsonObject(Payload);
    if (!JsonPayload.IsValid())
    {
        GB_BACKEND_LOG(Warning, TEXT("[Escape] Failed to convert payload to JSON"));
        return;
    }

    SendJsonMessage(TEXT("escape_request"), JsonPayload);
}
