#pragma once

#include "CoreMinimal.h"
#include "GBRightClickMenuTypes.generated.h"

UENUM(BlueprintType)
enum class ERightClickMenuAction : uint8
{
    None                    UMETA(DisplayName = "None"),

    // User
    InviteToParty           UMETA(DisplayName = "Invite to Party"),
    ViewProfile             UMETA(DisplayName = "View Profile"),
    AddToFriend             UMETA(DisplayName = "Add to Friend"),
    RemoveFriend            UMETA(DisplayName = "Remove from Friend"),
    BlockUser               UMETA(DisplayName = "Block"),
    UnblockUser             UMETA(DisplayName = "Unblock"),
    AcceptFriendRequest     UMETA(DisplayName = "Accept Friend Request"),
    DeclineFriendRequest    UMETA(DisplayName = "Decline Friend Request"),

    // From Party List
    KickFromParty           UMETA(DisplayName = "Kick from party"),
    DelegatePartyHost       UMETA(DisplayName = "Delegate party host"),
    LeaveFromParty          UMETA(DisplayName = "Leave from party"),

    // Item
    UseItem                 UMETA(DisplayName = "Use Item"),
    EquipItem               UMETA(DisplayName = "Equip Item"),
    InspectItem             UMETA(DisplayName = "Inspect Item")
};

USTRUCT(BlueprintType)
struct FRightClickMenuEntry
{
    GENERATED_BODY()
    
    FRightClickMenuEntry() = default;

    FRightClickMenuEntry(ERightClickMenuAction InAction, const FText& InText)
        : Action(InAction), DisplayText(InText)
    {}
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ERightClickMenuAction    Action = ERightClickMenuAction::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText                    DisplayText;
};
