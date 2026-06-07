#pragma once

UENUM(BlueprintType)
enum class EGBLobbyPlayerListType : uint8
{
    Lobby,
    Friend,
    ReceivedFriendRequest,
    SentFriendRequest,
    Party
};
