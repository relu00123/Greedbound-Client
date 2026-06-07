// Fill out your copyright notice in the Description page of Project Settings.

#include "GBClientSocketSubsystem.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"
#include "Dom/JsonObject.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystem/Lobby/GBLobbyInitSubsystem.h"
#include "Subsystem/Network/Client/GBClientSocketRequestHelper.h"
#include "Subsystem/Network/Client/GBClientSocketResponseRegistrar.h"

void UGBClientSocketSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
    {
        FModuleManager::LoadModuleChecked<FWebSocketsModule>("WebSockets");
    }

    GBClientSocketResponseRegistrar::RegisterAll(this, GetGameInstance());

    UE_LOG(LogTemp, Log, TEXT("[SocketSubsystem] Initialized"));
}

void UGBClientSocketSubsystem::Deinitialize()
{
    if (Socket.IsValid() && Socket->IsConnected())
    {
        Socket->Close();
    }
    Socket = nullptr;

    MessageHandlers.Empty();

    UE_LOG(LogTemp, Log, TEXT("[SocketSubsystem] Deinitialized"));
}

void UGBClientSocketSubsystem::ConnectSocket(const FString& Token, const FString& NickName)
{
    FString Url = FString::Printf(TEXT("ws://localhost:3000/ws?token=%s&connecttype=client"), *Token);
    Socket = FWebSocketsModule::Get().CreateWebSocket(Url);

    Socket->OnConnected().AddLambda([this, NickName]() {
        UE_LOG(LogTemp, Log, TEXT(" WebSocket Connected"));

        // Socket이 연결되었을때 바인딩된 함수들 호출 
        OnSocketConnected.Broadcast();

        if (UGBLobbyInitSubsystem* LobbyInitSubSystem = GetGameInstance()->GetSubsystem<UGBLobbyInitSubsystem>())
            {
                // 로비 진입전 초기화 대상 설정
                LobbyInitSubSystem->SetTargetInitSteps(ELobbyInitStep::ALL);

                // 웹 소켓으로 정보 요청
                GBClientSocketRequestHelper::SendLobbyUserListRequest();
                GBClientSocketRequestHelper::SendFriendListRequest(NickName);
                GBClientSocketRequestHelper::SendFriendRequestSentListRequest(NickName);
                GBClientSocketRequestHelper::SendFriendRequestReceivedListRequest(NickName);
            }
        });

    Socket->OnConnectionError().AddLambda([](const FString& Error) {
        UE_LOG(LogTemp, Error, TEXT(" WebSocket Error: %s"), *Error);
        });

    Socket->OnClosed().AddLambda([](int32 StatusCode, const FString& Reason, bool bWasClean) {
        UE_LOG(LogTemp, Warning, TEXT(" WebSocket Closed: %s"), *Reason);
        });

    Socket->OnMessage().AddUObject(this, &UGBClientSocketSubsystem::HandleMessage);
    Socket->Connect();
}

bool UGBClientSocketSubsystem::IsConnected() const
{
    return Socket.IsValid() && Socket->IsConnected();;
}

void UGBClientSocketSubsystem::RegisterHandler(const FString& MessageType, TFunction<void(const TSharedPtr<FJsonObject>&)> Handler)
{
    MessageHandlers.Add(MessageType, Handler);
}

void UGBClientSocketSubsystem::ConnectToDedicatedServer(const FString& Host, int32 Port)
{
    UWorld* World = GetWorld(); // 또는 UGameplayStatics::GetGameInstance(this)->GetWorld();

    if (!World)
    {
        return;
    }

    FString Address = FString::Printf(TEXT("%s:%d"), *Host, Port);
    UE_LOG(LogTemp, Log, TEXT(" Tyring to Connect DedicatedServer: %s"), *Address);

    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT(" PlayerController does not exists."));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Trying To Connect Deicated Server AFter 30sec.."));

    FTimerDelegate TimerDelegate;
    TimerDelegate.BindLambda([PC, Address]()
        {
            UE_LOG(LogTemp, Log, TEXT("ClientTravel Executed, Address(hostPort) : %s"), *Address);
            PC->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
        });

    FTimerHandle TimerHandle;
    World->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 30.0f, false);
}

void UGBClientSocketSubsystem::SendJsonString(const FString& Message)
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

void UGBClientSocketSubsystem::HandleMessage(const FString& Message)
{
    UE_LOG(LogTemp, Warning, TEXT("======== Handle Meassage Called [GBClientSocketSubsystem.cpp] =========="));

    TSharedPtr<FJsonObject> Json;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Message);

    if (FJsonSerializer::Deserialize(Reader, Json) && Json.IsValid())
    {
        FString Type;
        if (Json->TryGetStringField(TEXT("type"), Type))
        {
            if (MessageHandlers.Contains(Type))
            {
                UE_LOG(LogTemp, Log, TEXT("Dispatching handler for type : %s"), *Type);
                MessageHandlers[Type](Json);
            }

            else if (Type == TEXT("matchSuccess"))
            {
                FString Host;
                int32 Port;
                FString DungeonId;

                if (Json->TryGetStringField(TEXT("host"), Host) &&
                    Json->TryGetNumberField(TEXT("port"), Port) &&
                    Json->TryGetStringField(TEXT("dungeonId"), DungeonId))
                {

                    UE_LOG(LogTemp, Warning, TEXT("==============================================="));
                    UE_LOG(LogTemp, Warning, TEXT("MatchSuccess! Trying To Transfer: %s:%d (%s)"), *Host, Port, *DungeonId);

                    ConnectToDedicatedServer(Host, Port);
                }

                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Something Wrong Happened!"));
                }
            }

            else if (Type == TEXT("escape_response"))
            {
                bool bSuccess = false;
                FString PacketMessage;

                // JSON에서 필드 추출
                if (Json->TryGetBoolField(TEXT("success"), bSuccess) &&
                    Json->TryGetStringField(TEXT("message"), PacketMessage))
                {
                    UE_LOG(LogTemp, Log, TEXT("[EscapeResponse] Success: %s, Message: %s"),
                        bSuccess ? TEXT("true") : TEXT("false"),
                        *PacketMessage);

                    // 예시: UI에 메시지 표시하거나 상태 업데이트
                    UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("/Game/GB/Maps/Lobby")));
                }
            }

            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Unknown JsonType. [GBClientSocketSubsystem.cpp]"));
            }
        }
    }
}
