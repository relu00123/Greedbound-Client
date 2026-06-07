// Fill out your copyright notice in the Description page of Project Settings.


#include "GB/Subsystem/Network/Client/GBClientSocketRequestHelper.h"
#include "GBClientSocketSubsystem.h"
#include "JsonObjectConverter.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Define/GBDefine.h"

void GBClientSocketRequestHelper::SendChangeClassRequest(EGBCharacterClassType RequestedClass)
{
    GB_BACKEND_LOG(Warning, TEXT("SendChangeClassRequest Called!"));
    TSharedPtr<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetNumberField(TEXT("RequestedClass"), static_cast<int32>(RequestedClass));

    SendJsonRequest(TEXT("ChangeClassRequest"), Payload);
}

void GBClientSocketRequestHelper::SendChangeMyUserInfo(const FUserInfo& MyUserInfo)
{
    TSharedPtr<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(TEXT("NickName"), MyUserInfo.NickName);
    Payload->SetNumberField(TEXT("ClassType"), static_cast<int32>(MyUserInfo.ClassType));
    Payload->SetBoolField(TEXT("bOnline"), MyUserInfo.bOnline);

    // 메시지 타입 명은 서버와 합의된 걸 사용
    SendJsonRequest(TEXT("ChangeMyUserInfo"), Payload);
}

void GBClientSocketRequestHelper::SendLobbyUserListRequest()
{
    SendJsonRequest(TEXT("LobbyUserListRequest"));
}

void GBClientSocketRequestHelper::SendFriendListRequest(const FString& MyNickName)
{
    TSharedPtr<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(TEXT("MyNickName"), MyNickName);

    SendJsonRequest(TEXT("FriendListRequest"), Payload);
}

void GBClientSocketRequestHelper::SendFriendRequestSentListRequest(const FString& MyNickName)
{
    TSharedPtr<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(TEXT("MyNickName"), MyNickName);

    SendJsonRequest(TEXT("FriendRequestSentListRequest"), Payload);
}

void GBClientSocketRequestHelper::SendFriendRequestReceivedListRequest(const FString& MyNickName)
{
    TSharedPtr<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(TEXT("MyNickName"), MyNickName);

    SendJsonRequest(TEXT("FriendRequestReceivedListRequest"), Payload);
}

void GBClientSocketRequestHelper::SendAddFriendRequest(const FString &MyNickName, const FString& TargetNickname)
{
    TSharedPtr<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(TEXT("MyNickname"), MyNickName);
    Payload->SetStringField(TEXT("TargetNickname"), TargetNickname);

    SendJsonRequest(TEXT("AddFriendRequest"), Payload);
}

void GBClientSocketRequestHelper::SendRemoveFriendRequest(const FString& MyNickName, const FString& TargetNickname)
{
    TSharedPtr<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(TEXT("MyNickname"), MyNickName);
    Payload->SetStringField(TEXT("TargetNickname"), TargetNickname);

    SendJsonRequest(TEXT("RemoveFriendRequest"), Payload);
}

void GBClientSocketRequestHelper::SendWithdrawFriendRequest(const FString& MyNickName, const FString& TargetNickname)
{
    TSharedPtr<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(TEXT("MyNickname"), MyNickName);
    Payload->SetStringField(TEXT("TargetNickname"), TargetNickname);

    SendJsonRequest(TEXT("WithdrawFriendRequest"), Payload);
}

void GBClientSocketRequestHelper::SendRespondToFriendRequest(const FString& MyNickName, const FString& TargetNickname, bool isAccepted)
{
    TSharedPtr<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(TEXT("MyNickname"), MyNickName);
    Payload->SetStringField(TEXT("TargetNickname"), TargetNickname);
    Payload->SetBoolField(TEXT("IsAccepted"), isAccepted);

    SendJsonRequest(TEXT("RespondToFriendRequest"), Payload);
}

void GBClientSocketRequestHelper::SendPartyInviteRequest(const FString& MyNickName, const FString& TargetNickname)
{
    TSharedPtr<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(TEXT("inviterName"), MyNickName);
    Payload->SetStringField(TEXT("inviteeName"), TargetNickname);

    SendJsonRequest(TEXT("InviteToParty"), Payload);
}

void GBClientSocketRequestHelper::SendPartyHostTransferRequest(const FString& HostNickName, const FString& TargetNickName)
{
    TSharedPtr<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(TEXT("HostNickName"), HostNickName);
    Payload->SetStringField(TEXT("TargetNickName"), TargetNickName);

    SendJsonRequest(TEXT("PartyHostTransfer"), Payload);
}

void GBClientSocketRequestHelper::SendPartyInviteAcceptRequest(const FString& InviterName)
{
    TSharedPtr<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(TEXT("inviterName"), InviterName);

    SendJsonRequest(TEXT("PartyInviteAccept"), Payload);
}

void GBClientSocketRequestHelper::SendKickFromPartyRequest(const FString& HostNickName, const FString& TargetNickName)
{
    TSharedPtr<FJsonObject> Payload = MakeShared<FJsonObject>();

    Payload->SetStringField(TEXT("HostNickName"), HostNickName);
    Payload->SetStringField(TEXT("TargetNickName"), TargetNickName);

    SendJsonRequest(TEXT("KickFromParty"), Payload);
}

void GBClientSocketRequestHelper::SendLeaveFromPartyRequest(const FString& MyNickName)
{
    TSharedPtr<FJsonObject> Payload = MakeShared<FJsonObject>();

    SendJsonRequest(TEXT("LeaveFromParty"), Payload);
}

void GBClientSocketRequestHelper::SendGamePhaseChangeRequest(const FString& NewPhase)
{
    TSharedPtr<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(TEXT("NewPhase"), NewPhase);

    SendJsonRequest(TEXT("GamePhaseChangeRequest"), Payload);
}

void GBClientSocketRequestHelper::SendMatchStartRequest(int32 MapNumericID)
{
    GB_BACKEND_LOG(Warning, TEXT("SendMatchStartRequest Called! "));

    TSharedPtr<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetNumberField(TEXT("MapNumericID"), MapNumericID);

    SendJsonRequest(TEXT("MatchStartRequest"), Payload);
}

void GBClientSocketRequestHelper::SendMatchCancelRequest(int32 MapNumericID)
{
    GB_BACKEND_LOG(Warning, TEXT("Send Match Cancel Request Called! "));

    TSharedPtr<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetNumberField(TEXT("MapNumericID"), MapNumericID);

    SendJsonRequest(TEXT("MatchCancelRequest"), Payload);
}

void GBClientSocketRequestHelper::SendJsonRequest(const FString& Type, const TSharedPtr<FJsonObject>& Payload)
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

    UGBClientSocketSubsystem* SocketSubsystem = GameInstance->GetSubsystem<UGBClientSocketSubsystem>();
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
