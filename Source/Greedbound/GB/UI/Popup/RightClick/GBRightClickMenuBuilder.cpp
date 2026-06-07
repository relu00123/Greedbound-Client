
#include "GBRightClickMenuBuilder.h"
#include "Subsystem/Social/GBFriendManager.h"
#include "Subsystem/Social/GBPartyManager.h"
#include "Subsystem/Lobby/GBLobbyUserManager.h"
#include "Define/User/UserInfo.h"

TArray<FRightClickMenuEntry> GBRightClickMenuBuilder::BuildForUser(const FUserInfo& TargetUser, const UGameInstance* GameInstance, EGBLobbyPlayerListType PlayerListType, bool isEmptySlot)
{
    TArray<FRightClickMenuEntry> OutEntries;

    if (!GameInstance) return OutEntries;

    const FString& NickName = TargetUser.NickName;

    const UGBLobbyUserManager* LobbyUserMgr = GameInstance->GetSubsystem<UGBLobbyUserManager>();
    const UGBFriendManager* FriendMgr = GameInstance->GetSubsystem<UGBFriendManager>();
    UGBPartyManager* PartyMgr = GameInstance->GetSubsystem<UGBPartyManager>();

    const bool bIsFriend = FriendMgr && FriendMgr->IsFriend(NickName);
    const bool bIsInMyParty = PartyMgr && PartyMgr->IsInParty(NickName);

    // const bool bIsInParty = PartyMgr && PartyMgr->IsInMyParty(NickName); // 함수는 예시

    // 프로필 보기 아마 구현 안할듯? 
    // OutEntries.Add({ ERightClickMenuAction::ViewProfile, FText::FromString(TEXT("프로필 보기")) });

    switch (PlayerListType)
    {
    case EGBLobbyPlayerListType::Lobby:
    {
        // 친구 관련 항목
        if (!isEmptySlot && (NickName != LobbyUserMgr->GetMyUserInfo()->NickName))
        {
            UE_LOG(LogTemp, Warning, TEXT("%s , %s"), *NickName, *(LobbyUserMgr->GetMyUserInfo()->NickName));

            if (bIsFriend)
            {
                OutEntries.Add({ ERightClickMenuAction::RemoveFriend, FText::FromString(TEXT("Delete Friend")) });
            }

            else
            {
                OutEntries.Add({ ERightClickMenuAction::AddToFriend, FText::FromString(TEXT("Send Friend Request")) });
            }

            if (!bIsInMyParty)
            {
                // 해당 유저를 내 파티에 초대할 수 있도록 한다. 
                OutEntries.Add({ ERightClickMenuAction::InviteToParty, FText::FromString(TEXT("Invite To Party")) });
            }
        }
    }
    break;

    case EGBLobbyPlayerListType::Friend:
    {
        if (!isEmptySlot)
        {
            OutEntries.Add({ ERightClickMenuAction::RemoveFriend, FText::FromString(TEXT("Delete Frined")) });

            if (!bIsInMyParty)
            {
                // 해당 유저를 내 파티에 초대할 수 있도록 한다. 
                OutEntries.Add({ ERightClickMenuAction::InviteToParty, FText::FromString(TEXT("Invite To Party")) });
            }
        }
    }
    break;

    case EGBLobbyPlayerListType::ReceivedFriendRequest:
    {
        if (!isEmptySlot)
        {
            OutEntries.Add({ ERightClickMenuAction::AcceptFriendRequest, FText::FromString(TEXT("Accept Friend Request")) });
            OutEntries.Add({ ERightClickMenuAction::DeclineFriendRequest, FText::FromString(TEXT("Decline Friend Request")) });
        }
    }
    break;

    case EGBLobbyPlayerListType::SentFriendRequest:
    {

        break;
    }

    case EGBLobbyPlayerListType::Party:
    {
        if (!isEmptySlot)
        {
            FPartyInfo CurrentPartyInfo = PartyMgr->GetCurrentPartyInfo();

            // 파티가 있는 경우 
            if (CurrentPartyInfo.IsValid())
            {
                // 접속한 클라이언트의 Slot인 경우
                if (LobbyUserMgr->GetMyUserInfo()->NickName == NickName)
                {
                    // 파티에서 떠나기 기능 추가 필요 
                    OutEntries.Add({ ERightClickMenuAction::LeaveFromParty, FText::FromString(TEXT("Leave from party")) });
                }

                // 접속한 클라이언트의 슬롯이 아닌 경우 
                else
                {
                    // 접속한 클라이언트가 파티장일 경우
                    if (CurrentPartyInfo.HostName == LobbyUserMgr->GetMyUserInfo()->NickName)
                    {
                        // 파티장 넘기기 기능 추가 필요
                        OutEntries.Add({ ERightClickMenuAction::DelegatePartyHost, FText::FromString(TEXT("Delegate party Host")) });
                        // 파티에서 추방하기 기능 추가 필요 
                        OutEntries.Add({ ERightClickMenuAction::KickFromParty, FText::FromString(TEXT("Kick from party")) });
                    }
                }
            }
        }

        // 해당 Slot이 자기 자신인지 판단 해야함 
        // 자신일경우 (파티 떠나기 )
        break;
    }

    default:
        break;
    }




    return OutEntries;
}
