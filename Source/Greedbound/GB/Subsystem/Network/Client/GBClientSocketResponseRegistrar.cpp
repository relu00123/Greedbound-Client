// Fill out your copyright notice in the Description page of Project Settings.


#include "GB/Subsystem/Network/Client/GBClientSocketResponseRegistrar.h"
#include "GBClientSocketResponseRegistrar.h"
#include "Subsystem/Network/Client/GBClientSocketSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystem/Lobby/GBLobbyUserManager.h"
#include "Subsystem/Lobby/GBLobbyInitSubsystem.h"
#include "Subsystem/Social/GBFriendManager.h"
#include "Subsystem/Network/Client/GBClientSessionSubsystem.h"
#include "Utils/GBThreadingUtils.h"
#include "Subsystem/UI/GBPopupManagerSubsystem.h"
#include "Subsystem/Social/GBPartyManager.h"
#include "Utils/GBJsonUtils.h"
#include "Subsystem/Matching/GBClientMatchQueueSubsystem.h"
#include "Subsystem/GamePhase/GBClientPhaseSubsystem.h"
#include "Subsystem/Matching/GBDungeonJoinSubsystem.h"
#include "Define/Match/GBDungeonJoinTypes.h"
#include "Define/GamePhase/GBClientGamePhase.h"
#include "Define/GBDefine.h"

void GBClientSocketResponseRegistrar::RegisterAll(UGBClientSocketSubsystem* Socket, UGameInstance* GameInstance)
{
    if (!Socket)
    {
        return;
    }

    RegisterLobbyHandlers(Socket, GameInstance);
    RegisterFriendHandlers(Socket, GameInstance);
    RegisterMatchHandlers(Socket, GameInstance);
    RegisterPartyHandlers(Socket, GameInstance);
    RegisterGamePhaseHandlers(Socket, GameInstance);
}

void GBClientSocketResponseRegistrar::RegisterLobbyHandlers(UGBClientSocketSubsystem* Socket, UGameInstance* GameInstance)
{
    Socket->RegisterHandler(TEXT("LobbyUserListResponse"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json)
        {
            const TArray<TSharedPtr<FJsonValue>>* UserArray;
            if (Json->TryGetArrayField(TEXT("payload"), UserArray))
            {
                if (UGameInstance* gameInstance = UGameplayStatics::GetGameInstance(GWorld))
                {
                    UGBLobbyUserManager* UserManager = gameInstance->GetSubsystem<UGBLobbyUserManager>();
                    check(UserManager);

                    for (const auto& Value : *UserArray)
                    {
                        const TSharedPtr<FJsonObject> UserObj = Value->AsObject();

                        FString UserName = UserObj->GetStringField(TEXT("username"));

                        // Class Type
                        FString ClassTypeString = UserObj->GetStringField(TEXT("classType"));

                        EGBCharacterClassType ParsedClassType = EGBCharacterClassType::None;
                        UEnum* EnumPtr = StaticEnum<EGBCharacterClassType>();
                        if (EnumPtr)
                        {
                            ParsedClassType = static_cast<EGBCharacterClassType>(EnumPtr->GetValueByNameString(ClassTypeString));
                        }

                        // Game Phase
                        FString PhaseStr;
                        EGBClientGamePhase GamePhase = EGBClientGamePhase::None;

                        if (UserObj->TryGetStringField(TEXT("gamePhase"), PhaseStr)) 
                        {
                            // 문자열 -> enum 매핑 (이름 정확히 일치 필요)
                            if (UEnum* PhaseEnum = StaticEnum<EGBClientGamePhase>())
                            {
                                const int64 RawVal = PhaseEnum->GetValueByNameString(PhaseStr); // "Lobby" 등
                                if (RawVal != INDEX_NONE) 
                                {
                                    GamePhase = static_cast<EGBClientGamePhase>(RawVal);
                                    GB_BACKEND_LOG(Warning, TEXT("GamePhase changed (string): %s"), *UEnum::GetValueAsString(GamePhase));

                                    UserManager->SetGamePhase(UserName, GamePhase);  
                                }
                                else 
                                {
                                    GB_BACKEND_LOG(Warning, TEXT("Unknown GamePhase string: %s"), *PhaseStr);
                                }
                            }
                        }



                        FUserInfo NewUser;
                        NewUser.NickName = UserName;
                        NewUser.ClassType = ParsedClassType;
                        NewUser.bOnline = true;
                        NewUser.ClientGamePhase = GamePhase;

                        UserManager->AddUser(NewUser);
                        GB_BACKEND_LOG(Log, TEXT("[LobbyUserList] %s (%s)"), *UserName, *ClassTypeString);
                    }
                }
            }

            // lobby 유저목록을 받은 시점이기 때문에 LobbyInitSubSystem에서 LobbyUserList부분 준비완료를 해준다.
            UGBLobbyInitSubsystem* LobbyInitSubsystem = GameInstance->GetSubsystem<UGBLobbyInitSubsystem>();
            LobbyInitSubsystem->MarkStepCompleted(ELobbyInitStep::PlayerList);
        });

    Socket->RegisterHandler(TEXT("playerJoined"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json)
        {
            GB_BACKEND_LOG(Warning, TEXT("+++++++++++++++++ PlayerJoined +++++++++++++++++++++++"));

            const TSharedPtr<FJsonObject>* PayloadObj;
            if (Json->TryGetObjectField(TEXT("payload"), PayloadObj))
            {
                FString UserName = PayloadObj->Get()->GetStringField(TEXT("userName"));
                FString ClassTypeString = PayloadObj->Get()->GetStringField(TEXT("classType"));

                EGBCharacterClassType ParsedClassType = EGBCharacterClassType::None;
                if (UEnum* EnumPtr = StaticEnum<EGBCharacterClassType>())
                {
                    ParsedClassType = static_cast<EGBCharacterClassType>(EnumPtr->GetValueByNameString(ClassTypeString));
                }

                // Game Phase
                FString PhaseStr;
                EGBClientGamePhase GamePhase = EGBClientGamePhase::GameInitializing;

                if (PayloadObj->Get()->TryGetStringField(TEXT("gamePhase"), PhaseStr)) 
                {
                    // 문자열 -> enum 매핑 (이름 정확히 일치 필요)
                    if (UEnum* PhaseEnum = StaticEnum<EGBClientGamePhase>()) 
                    {
                        const int64 RawVal = PhaseEnum->GetValueByNameString(PhaseStr); // "Lobby" 등
                        if (RawVal != INDEX_NONE) 
                        {
                            GamePhase = static_cast<EGBClientGamePhase>(RawVal);
                            GB_BACKEND_LOG(Warning, TEXT("GamePhase changed (string): %s"), *UEnum::GetValueAsString(GamePhase));
                        }
                        else 
                        {
                            GB_BACKEND_LOG(Warning, TEXT("Unknown GamePhase string: %s"), *PhaseStr);
                        }
                    }
                }

                FUserInfo NewUser;
                NewUser.NickName = UserName;
                NewUser.ClassType = ParsedClassType;
                NewUser.bOnline = true;
                NewUser.ClientGamePhase = GamePhase;

                if (UGameInstance* gameInstance = UGameplayStatics::GetGameInstance(GWorld))
                {
                    UGBLobbyUserManager* UserManager = gameInstance->GetSubsystem<UGBLobbyUserManager>();
                    check(UserManager);

                    UserManager->AddUser(NewUser);
                }

                GB_BACKEND_LOG(Log, TEXT("[playerJoined] %s (%s)"), *UserName, *ClassTypeString);
            }
        });

    Socket->RegisterHandler(TEXT("playerLeft"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json)
        {
            const TSharedPtr<FJsonObject>* PayloadObj;
            if (Json->TryGetObjectField(TEXT("payload"), PayloadObj))
            {
                FString UserName = PayloadObj->Get()->GetStringField(TEXT("userName"));

                if (UGameInstance* gameInstance = UGameplayStatics::GetGameInstance(GWorld))
                {
                    UGBLobbyUserManager* UserManager = gameInstance->GetSubsystem<UGBLobbyUserManager>();
                    check(UserManager);

                    UserManager->RemoveUser(UserName);  // 유저 제거
                }

                GB_BACKEND_LOG(Log, TEXT("[playerLeft] %s"), *UserName);
            }
        });

    Socket->RegisterHandler(TEXT("ChangeClassResponse"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json) {
            GB_BACKEND_LOG(Warning, TEXT("Change Class Response Received!!!!!"));

            const TSharedPtr<FJsonObject>* PayloadObj = nullptr;
            if (Json->TryGetObjectField(TEXT("payload"), PayloadObj))
            {
                const TSharedPtr<FJsonObject>& Payload = *PayloadObj;

                bool bSuccess = false;
                Payload->TryGetBoolField(TEXT("success"), bSuccess);

                int32 OldClassInt = -1;
                int32 NewClassInt = -1;
                const bool bHasOld = Payload->TryGetNumberField(TEXT("oldClass"), OldClassInt);
                const bool bHasNew = Payload->TryGetNumberField(TEXT("newClass"), NewClassInt);

                FString ErrorStr;
                Payload->TryGetStringField(TEXT("error"), ErrorStr);
                FString NoteStr;
                Payload->TryGetStringField(TEXT("note"), NoteStr);

                auto IsValidClass = [](int32 V)->bool
                    {
                        return V >= static_cast<int32>(EGBCharacterClassType::None) &&
                            V <= static_cast<int32>(EGBCharacterClassType::Max);
                    };

                EGBCharacterClassType OldClass = EGBCharacterClassType::None;
                EGBCharacterClassType NewClass = EGBCharacterClassType::None;

                if (bHasOld && IsValidClass(OldClassInt))
                {
                    OldClass = static_cast<EGBCharacterClassType>(OldClassInt);
                }
                else
                {
                    // oldClass가 없거나 범위를 벗어나면 현재 값으로 대체하거나 로그만 남김
                    GB_BACKEND_LOG(Verbose, TEXT("ChangeClassResponse: oldClass missing/invalid."));
                }

                if (bHasNew && IsValidClass(NewClassInt))
                {
                    NewClass = static_cast<EGBCharacterClassType>(NewClassInt);
                }
                else
                {
                    GB_BACKEND_LOG(Verbose, TEXT("ChangeClassResponse: newClass missing/invalid."));
                }

                GB_BACKEND_LOG(Warning, TEXT("HandleChangeClassResponse Calling..."));
                UGBLobbyUserManager* UserManager =  GameInstance->GetSubsystem<UGBLobbyUserManager>();
                UserManager->HandleChangeClassResponse(bSuccess, OldClass, NewClass);
            }
        });

    Socket->RegisterHandler(TEXT("UserInfoUpdated"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json) {
            const TSharedPtr<FJsonObject>* PayloadObj = nullptr;

            Json->TryGetObjectField(TEXT("payload"), PayloadObj);
            const TSharedPtr<FJsonObject>& Payload = *PayloadObj;

            FString UserName;
            Payload->TryGetStringField(TEXT("username"), UserName);

            FString ClassName = TEXT("Unknown");
            Payload->TryGetStringField(TEXT("classType"), ClassName);

            int32 ClassEnumVal = 0;
            Payload->TryGetNumberField(TEXT("classTypeEnum"), ClassEnumVal);
            const EGBCharacterClassType ClassType = static_cast<EGBCharacterClassType>(ClassEnumVal);

            GB_BACKEND_LOG(Warning, TEXT("UserInfoUpdated Received!!! username=%s, class=%s (%d)"),
                *UserName, *ClassName, ClassEnumVal);

            FString PhaseStr;
            EGBClientGamePhase GamePhase = EGBClientGamePhase::None;

            if (PayloadObj->Get()->TryGetStringField(TEXT("gamePhase"), PhaseStr)) {
                // 문자열 -> enum 매핑 (이름 정확히 일치 필요)
                if (UEnum* PhaseEnum = StaticEnum<EGBClientGamePhase>()) 
                {
                    const int64 RawVal = PhaseEnum->GetValueByNameString(PhaseStr); // "Lobby" 등
                    if (RawVal != INDEX_NONE) 
                    {
                        GamePhase = static_cast<EGBClientGamePhase>(RawVal);
                        GB_BACKEND_LOG(Warning, TEXT("GamePhase changed (string): %s"), *UEnum::GetValueAsString(GamePhase));

                        UGBLobbyUserManager* LobbyUserManager = GameInstance->GetSubsystem<UGBLobbyUserManager>();

                        if (LobbyUserManager)
                        {
                            LobbyUserManager->SetGamePhase(UserName, GamePhase);

                        }
                    }
                    else 
                    {
                        GB_BACKEND_LOG(Warning, TEXT("Unknown GamePhase string: %s"), *PhaseStr);
                    }
                }
            }

            UGBLobbyUserManager* UserManager = GameInstance->GetSubsystem<UGBLobbyUserManager>();

            FUserInfo UpdatedUserInfo;
            UpdatedUserInfo.NickName = UserName;
            UpdatedUserInfo.ClassType = ClassType;
            UpdatedUserInfo.bOnline = true; // 음.. UserInfoUpdated가 일어나는 시점은 online이 ture여야해서 이렇게 하긴 했는데.. 
            UpdatedUserInfo.ClientGamePhase = GamePhase;

            GB::ThreadingUtils::RunOnGameThread([UserManager, UpdatedUserInfo]()
            {
                UserManager->HandleUpdateUserInfo(UpdatedUserInfo);

            });

            //GB::ThreadingUtils::RunOnGameThread([UserManager, ])

            //GB::ThreadingUtils::RunOnGameThread([UserMgr, UserName, ClassType]()
            //    {
            //        // (A) 특정 유저의 클래스만 갱신하는 API가 있으면 사용
            //        // UserMgr->UpdateUserClassByName(UserName, ClassType);

            //        // (B) 스냅샷 기반으로 갱신하는 방식이라면:
            //        FUserInfo NewInfo;
            //        NewInfo.NickName = UserName;     // 비어있으면 내부에서 무시/매칭 규칙 처리
            //        NewInfo.ClassType = ClassType;
            //        NewInfo.bOnline = true;

            //        // 프로젝트에 맞는 함수 하나로 통일해 두길 권장
            //        // 예: UserMgr->UpsertOrRefreshUser(NewInfo);
            //        // 없으면 SetMyUserInfo/SetOtherUserInfo로 분리해도 됨.
            //        UserMgr->UpsertOrRefreshUser(NewInfo);

            //        UE_LOG(LogTemp, Verbose, TEXT("UserInfoUpdated parsed: %s -> Class=%d"),
            //            *UserName, static_cast<int32>(ClassType));
            //    });
        });

    Socket->RegisterHandler(TEXT("PlayerSessionUpdated"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json) {

            UGBLobbyUserManager* LobbyUserManager = GameInstance->GetSubsystem<UGBLobbyUserManager>();
            UGBClientPhaseSubsystem* GamePhaseSubsystem = GameInstance->GetSubsystem<UGBClientPhaseSubsystem>();

            // UserName
            FString UserName;
            if (!Json->TryGetStringField(TEXT("userId"), UserName))
            {
                return;
            }


            const TSharedPtr<FJsonObject>* ChangedObj = nullptr;
            if (!Json->TryGetObjectField(TEXT("changed"), ChangedObj) || !ChangedObj)
            {
                return;
            }

            // patch 적용  
            
            // ==================== class Type ===================
            int32 ClassEnumVal = 0;
            EGBCharacterClassType ClassType;
            if ((*ChangedObj)->TryGetNumberField(TEXT("classType"), ClassEnumVal)) {
                ClassType = static_cast<EGBCharacterClassType>(ClassEnumVal);

                GB_BACKEND_LOG(Warning, TEXT("ClassType<> = %s"), *UEnum::GetValueAsString(ClassType));

                // LobbyManager에서 해당 유저에 대한 클래스를 변경해야 한다. 
                LobbyUserManager->SetClasss(UserName, ClassType);
            }
         
            GB_BACKEND_LOG(Warning, TEXT("UserInfoUpdated Received!!! , class=(%d)"),
                  ClassEnumVal);


            // ===================== Game Phase ===================
            {
                // 우선 문자열로 시도
                FString PhaseStr;

                if ((*ChangedObj)->TryGetStringField(TEXT("gamePhase"), PhaseStr)) {
                    // 문자열 -> enum 매핑 (이름 정확히 일치 필요)
                    if (UEnum* PhaseEnum = StaticEnum<EGBClientGamePhase>()) {
                        const int64 RawVal = PhaseEnum->GetValueByNameString(PhaseStr); // "Lobby" 등
                        if (RawVal != INDEX_NONE) 
                        {
                            const EGBClientGamePhase Phase = static_cast<EGBClientGamePhase>(RawVal);
                            GB_BACKEND_LOG(Warning, TEXT("GamePhase changed (string): %s"), *UEnum::GetValueAsString(Phase));

                            LobbyUserManager->SetGamePhase(UserName, Phase);   
                        }
                        else 
                        {
                            GB_BACKEND_LOG(Warning, TEXT("Unknown GamePhase string: %s"), *PhaseStr);
                        }
                    }
                }
            }


            // patch 적용
            //FString Nick;
            //if ((*ChangedObj)->TryGetStringField(TEXT("username"), Nick))
            //    Info.NickName = Nick;

            //int32 ClassEnumVal;
            //if ((*ChangedObj)->TryGetNumberField(TEXT("classTypeEnum"), ClassEnumVal))
            //    Info.ClassType = static_cast<EGBCharacterClassType>(ClassEnumVal);

            //FString ClassStr;
            //if ((*ChangedObj)->TryGetStringField(TEXT("classType"), ClassStr))
            //    Info.ClassType = StringToClassType(ClassStr); // 문자열을 enum으로 바꾸는 헬퍼

            //FString PartyId;
            //if ((*ChangedObj)->TryGetStringField(TEXT("partyId"), PartyId))
            //{
            //    // 파티 정보 업데이트
            //}

            //bool OnlineVal;
            //if ((*ChangedObj)->TryGetBoolField(TEXT("online"), OnlineVal))
            //    Info.bOnline = OnlineVal;

            //// UI 업데이트용 delegate 호출
            //OnUserUpdated.Broadcast(Info);

        });
}

void GBClientSocketResponseRegistrar::RegisterFriendHandlers(UGBClientSocketSubsystem* Socket, UGameInstance* GameInstance)
{
    Socket->RegisterHandler(TEXT("FriendListResponse"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json)
        {
            const TArray<TSharedPtr<FJsonValue>>* payloadArray;
            if (Json->TryGetArrayField(TEXT("payload"), payloadArray))
            {
                UGBFriendManager* FriendManager = GameInstance->GetSubsystem<UGBFriendManager>();
                check(FriendManager);

                for (const TSharedPtr<FJsonValue>& Value : *payloadArray)
                {
                    const TSharedPtr<FJsonObject>* EntryObject;
                    if (Value->TryGetObject(EntryObject))
                    {
                        FString FriendID;
                        if ((*EntryObject)->TryGetStringField(TEXT("friend_id"), FriendID))
                        {
                            FriendManager->AddFriend(FriendID);
                        }
                    }
                }

                // 친구목록을 받은 시점이기 때문에 LobbyInitSubSystem에서 FriendList부분 준비완료를 해준다.
                UGBLobbyInitSubsystem* LobbyInitSubsystem = GameInstance->GetSubsystem<UGBLobbyInitSubsystem>();
                LobbyInitSubsystem->MarkStepCompleted(ELobbyInitStep::FriendList);
            }

            else
            {
                GB_BACKEND_LOG(Error, TEXT("Failed to parse payload array in FriendListResponse"));
            }
        });

    Socket->RegisterHandler(TEXT("FriendRequestSentListResponse"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json)
        {
            const TArray<TSharedPtr<FJsonValue>>* payloadArray;

            if (Json->TryGetArrayField(TEXT("payload"), payloadArray))
            {
                UGBFriendManager* FriendManager = GameInstance->GetSubsystem<UGBFriendManager>();
                check(FriendManager);

                for (const TSharedPtr<FJsonValue>& Value : *payloadArray)
                {
                    const TSharedPtr<FJsonObject>* EntryObject;
                    if (Value->TryGetObject(EntryObject))
                    {
                        FString SentFrinedID;
                        if ((*EntryObject)->TryGetStringField(TEXT("friend_id"), SentFrinedID))
                        {
                            GB_BACKEND_LOG(Log, TEXT("Sent Friend Request : me -> %s"), *SentFrinedID);
                            FriendManager->AddSentFriendRequest(SentFrinedID);
                        }
                    }
                }

                // 보낸 친구목록을 받은 시점이기 때문에 LobbyInitSubSystem에서 FriendRequestSentList부분 준비완료를 해준다.
                UGBLobbyInitSubsystem* LobbyInitSubsystem = GameInstance->GetSubsystem<UGBLobbyInitSubsystem>();
                LobbyInitSubsystem->MarkStepCompleted(ELobbyInitStep::FriendRequestSentList);
            }

            else
            {
                GB_BACKEND_LOG(Error, TEXT("Failed to parse payload array in FriendRequestSentListResponse"));
            }
        });

    Socket->RegisterHandler(TEXT("AddFriendRequestSentResponse"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json)
        {
            TSharedPtr<FJsonObject> Payload = Json->GetObjectField(TEXT("payload"));
            if (!Payload.IsValid())
            {
                return;
            }

            // isSucceed 파싱
            bool bIsSucceed = false;
            if (Payload->HasTypedField<EJson::Boolean>(TEXT("isSucceed")))
            {
                bIsSucceed = Payload->GetBoolField(TEXT("isSucceed"));
            }

            // sentPacket 파싱
            FString FriendId;
            if (Payload->HasField(TEXT("sentPacket")))
            {
                TSharedPtr<FJsonObject> SentPacket = Payload->GetObjectField(TEXT("sentPacket"));
                if (SentPacket.IsValid() && SentPacket->HasField(TEXT("friend_id")))
                {
                    FriendId = SentPacket->GetStringField(TEXT("friend_id"));
                }
            }

            // 결과 확인용 로그 출력
            GB_BACKEND_LOG(Log, TEXT("AddFriendRequestSentResponse received: success=%s, friend_id=%s"),
                bIsSucceed ? TEXT("true") : TEXT("false"),
                *FriendId);

            // TODO: GameInstance에 친구 추가 처리 등 구현
            UGBFriendManager* FriendManager = GameInstance->GetSubsystem<UGBFriendManager>();
            if (bIsSucceed &&  FriendManager)
            {
                FriendManager->AddSentFriendRequest(FriendId);
            }
        });

    Socket->RegisterHandler(TEXT("FriendRemoved"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json) 
        {
            if (!Json.IsValid())
            {
                GB_BACKEND_LOG(Error, TEXT("FriendRemoved: invalid JSON"));
                return;
            }

            // payload 객체 꺼내기 (소문자 키 사용)
            const TSharedPtr<FJsonObject>* PayloadObjPtr = nullptr;
            if (!Json->TryGetObjectField(TEXT("payload"), PayloadObjPtr))
            {
                // 혹시 대문자일 수도 있으면 보조 체크
                if (!Json->TryGetObjectField(TEXT("Payload"), PayloadObjPtr))
                {
                    GB_BACKEND_LOG(Error, TEXT("FriendRemoved: payload missing"));
                    return;
                }
            }

            FString TargetId;
            if (!(*PayloadObjPtr)->TryGetStringField(TEXT("targetID"), TargetId))
            {
                GB_BACKEND_LOG(Error, TEXT("FriendRemoved: targetID missing"));
                return;
            }

            GB_BACKEND_LOG(Log, TEXT("FriendRemoved: targetID=%s"), *TargetId);

            if (auto* FriendManager = GameInstance->GetSubsystem<UGBFriendManager>())
            {
                FriendManager->RemoveFriend(TargetId);
            }

        });

    Socket->RegisterHandler(TEXT("FriendRequestReceivedListResponse"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json)
        {
            const TArray<TSharedPtr<FJsonValue>>* payloadArray;

            if (Json->TryGetArrayField(TEXT("payload"), payloadArray))
            {
                UGBFriendManager* FriendManager = GameInstance->GetSubsystem<UGBFriendManager>();
                check(FriendManager);

                for (const TSharedPtr<FJsonValue>& Value : *payloadArray)
                {
                    const TSharedPtr<FJsonObject>* EntryObject;
                    if (Value->TryGetObject(EntryObject))
                    {
                        FString ReceivedFriendID;
                        if ((*EntryObject)->TryGetStringField(TEXT("user_id"), ReceivedFriendID))
                        {
                            // 로직 작성해야함!! 
                            UE_LOG(LogTemp, Log, TEXT("Received Friend Request : %s -> me"), *ReceivedFriendID);
                            FriendManager->AddReceivedFriendRequest(ReceivedFriendID);
                        }
                    }
                }

                // 받은 친구목록을 받은 시점이기 때문에 LobbyInitSubSystem에서 FriendRequestReceivedList부분 준비완료를 해준다.
                UGBLobbyInitSubsystem* LobbyInitSubsystem = GameInstance->GetSubsystem<UGBLobbyInitSubsystem>();
                LobbyInitSubsystem->MarkStepCompleted(ELobbyInitStep::FriendRequestReceivedList);
            }

            else
            {
                GB_BACKEND_LOG(Error, TEXT("Failed to parse payload array in FriendRequestSentListResponse"));
            }
        });

    Socket->RegisterHandler(TEXT("DeleteSentFriendRequest"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json)
        {
            UGBFriendManager* FriendManager = GameInstance->GetSubsystem<UGBFriendManager>();
            check(FriendManager);

            const FString DeletingUserId = Json->GetStringField(TEXT("payload"));
            FriendManager->RemoveSentFriendRequest(DeletingUserId);
        });

    Socket->RegisterHandler(TEXT("FriendRequestReceived"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json)
        {
            if (!Json.IsValid())
            {
                return;
            }

            // payload 꺼내기
            const TSharedPtr<FJsonObject>* PayloadObj;
            if (Json->TryGetObjectField(TEXT("payload"), PayloadObj))
            {
                const TSharedPtr<FJsonObject>& Payload = *PayloadObj;

                UGBFriendManager* FriendManager = GameInstance->GetSubsystem<UGBFriendManager>();

                // user_id 파싱
                FString UserId;
                if (Payload->TryGetStringField(TEXT("user_id"), UserId))
                {
                    GB_BACKEND_LOG(Log, TEXT("[FriendRequestReceived] user_id: %s"), *UserId);

                    if (FriendManager)
                    {
                        GB_BACKEND_LOG(Warning, TEXT("Trying To Handle AddReceivedFriendRequest!"));
                        FriendManager->AddReceivedFriendRequest(UserId);
                    }
                }
            }
        });

    Socket->RegisterHandler(TEXT("DeleteReceivedFriendRequest"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json)
        {
            UGBFriendManager* FriendManager = GameInstance->GetSubsystem<UGBFriendManager>();
            check(FriendManager);

            const FString DeletingUserId = Json->GetStringField(TEXT("payload"));
            FriendManager->RemoveReceivedFriendRequest(DeletingUserId);
        });

    Socket->RegisterHandler(TEXT("FriendRequestAccepted"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json)
        {
            UGBFriendManager* FriendManager = GameInstance->GetSubsystem<UGBFriendManager>();
            check(FriendManager);

            // payload 오브젝트 가져오기
            const TSharedPtr<FJsonObject>* PayloadObj;
            if (Json->TryGetObjectField(TEXT("payload"), PayloadObj))
            {
                const FString AcceptedBy = (*PayloadObj)->GetStringField(TEXT("AcceptedBy"));
                const FString AcceptedTo = (*PayloadObj)->GetStringField(TEXT("AcceptedTo"));

                // 로그 출력
                GB_BACKEND_LOG(Log, TEXT("FriendRequestAccepted - AcceptedBy: %s, AcceptedTo: %s"), *AcceptedBy, *AcceptedTo);

                UGBClientSessionSubsystem* ClientSessionSubsystem = GameInstance->GetSubsystem<UGBClientSessionSubsystem>();
                const FString MyNickName = ClientSessionSubsystem->GetNickname();

                if (AcceptedBy == MyNickName)
                {
                    FriendManager->RemoveReceivedFriendRequest(AcceptedTo);
                    FriendManager->AddFriend(AcceptedTo);
                }

                else if (AcceptedTo == MyNickName)
                {
                    FriendManager->RemoveSentFriendRequest(AcceptedBy);
                    FriendManager->AddFriend(AcceptedBy);
                }

                else
                {
                    GB_BACKEND_LOG(Error, TEXT("RejectedBy or RejectedTo does not match with my Nickname"));
                }
            }
            else
            {
                GB_BACKEND_LOG(Warning, TEXT("Failed to parse payload from FriendRequestAccepted"));
            }
        });

    Socket->RegisterHandler(TEXT("FriendRequestRejected"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json)
        {
            UGBFriendManager* FriendManager = GameInstance->GetSubsystem<UGBFriendManager>();
            check(FriendManager);

            // payload 객체 추출
            const TSharedPtr<FJsonObject>* PayloadObj;
            if (Json->TryGetObjectField(TEXT("payload"), PayloadObj))
            {
                const FString RejectedBy = (*PayloadObj)->GetStringField(TEXT("rejectedBy"));
                const FString RejectedTo = (*PayloadObj)->GetStringField(TEXT("rejectedTo"));

                // 로그 출력
                GB_BACKEND_LOG(Log, TEXT("FriendRequestRejected - RejectedBy: %s, RejectedTo: %s"), *RejectedBy, *RejectedTo);

                UGBClientSessionSubsystem* ClientSessionSubsystem = GameInstance->GetSubsystem<UGBClientSessionSubsystem>();
                const FString MyNickName = ClientSessionSubsystem->GetNickname();

                if (RejectedBy == MyNickName)
                {
                    FriendManager->RemoveReceivedFriendRequest(RejectedTo);
                }

                else if (RejectedTo == MyNickName)
                {
                    FriendManager->RemoveSentFriendRequest(RejectedBy);
                }

                else
                {
                    GB_BACKEND_LOG(Error, TEXT("RejectedBy or RejectedTo does not match with my Nickname"));
                }
            }
            else
            {
                GB_BACKEND_LOG(Warning, TEXT("Failed to parse payload from FriendRequestRejected"));
            }
        });
    
}

void GBClientSocketResponseRegistrar::RegisterMatchHandlers(UGBClientSocketSubsystem* Socket, UGameInstance* GameInstance)
{
    Socket->RegisterHandler(TEXT("QueueJoined"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json)
        {
            if (!Json.IsValid()) return;

            // payload
            const TSharedPtr<FJsonObject>* PayloadObj = nullptr;
            if (!Json->TryGetObjectField(TEXT("payload"), PayloadObj)) return;
            const TSharedPtr<FJsonObject>& Payload = *PayloadObj;

            // mapId (mapId OR GameMapNumericId 허용)
            int32 MapId = 0;
            Payload->TryGetNumberField(TEXT("mapId"), MapId);

            // policy
            FString PolicyStr;
            Payload->TryGetStringField(TEXT("policy"), PolicyStr);
            const EJoinPolicy Policy = GBMatch::ParsePolicy(PolicyStr);

            // ticketId
            FString TicketId;
            Payload->TryGetStringField(TEXT("ticketId"), TicketId);

            // partyId (없으면 0 = solo)
            int32 PartyId = 0;
            Payload->TryGetNumberField(TEXT("partyId"), PartyId);

            GB_BACKEND_LOG(Warning, TEXT("[QueueJoined] map=%d policy=%s ticket=%s party=%d"), MapId, *PolicyStr, *TicketId, PartyId);

            if (UGBClientMatchQueueSubsystem* MQ = GameInstance->GetSubsystem<UGBClientMatchQueueSubsystem>())
            {
                MQ->HandleQueueJoined(MapId, Policy, TicketId, PartyId);
            }
             
        });

    Socket->RegisterHandler(TEXT("QueueCanceled"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json)
        {
            if (!Json.IsValid()) 
            { 
                return;
            }

            // payload
            const TSharedPtr<FJsonObject>* PayloadObj = nullptr;
            if (!Json->TryGetObjectField(TEXT("payload"), PayloadObj))
            {
                return;
            }
            const TSharedPtr<FJsonObject>& Payload = *PayloadObj;

            // mapId (mapId OR GameMapNumericId 허용)
            int32 MapId = 0;
            Payload->TryGetNumberField(TEXT("mapId"), MapId);

            // reason
            //FString Reason;
            //Payload->TryGetStringField(TEXT("reason"), Reason);

            // partyId (없으면 0 = solo)
            int32 PartyId = 0;
            Payload->TryGetNumberField(TEXT("partyId"), PartyId);

            GB_BACKEND_LOG(Warning ,TEXT("[QueueCanceled] map=%d party=%d"), MapId, PartyId);

            if (UGBClientMatchQueueSubsystem* MQ = GameInstance->GetSubsystem<UGBClientMatchQueueSubsystem>())
            {
                MQ->HandleQueueCanceled(MapId, PartyId);
            }

        });

    // A) MatchAssigned
    Socket->RegisterHandler(TEXT("MatchAssigned"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json)
        {
            const TSharedPtr<FJsonObject>* PObj = nullptr;
            if (!Json.IsValid() || !Json->TryGetObjectField(TEXT("payload"), PObj))
            {
                return;
            }

            const TSharedPtr<FJsonObject>& P = *PObj;

            int32 MapId = 0; FString MatchId, TeamId;
            TArray<FString> TeamMembers;

            P->TryGetNumberField(TEXT("mapId"), MapId);
            P->TryGetStringField(TEXT("matchId"), MatchId);

            // 서버가 더이상 안줄 수도 잇음. (teamID와 teamMembers에 대해서 MatchAssigned에서는 단순 정보만 준다) 
            P->TryGetStringField(TEXT("teamId"), TeamId);

            const TArray<TSharedPtr<FJsonValue>>* Arr = nullptr;
            if (P->TryGetArrayField(TEXT("teamMembers"), Arr)) 
            {
                for (const auto& v : *Arr) 
                { 
                    FString S; if (v->TryGetString(S)) TeamMembers.Add(S); 
                }
            }

            if (auto* Sub = GameInstance->GetSubsystem<UGBClientMatchQueueSubsystem>()) 
            {
                Sub->HandleMatchAssigned(MapId, MatchId, TeamId, TeamMembers);
            }
        });

    // DungeonReady - Legacy 
    Socket->RegisterHandler(TEXT("DungeonReady"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json)
        {
            const TSharedPtr<FJsonObject>* PObj = nullptr;
            if (!Json.IsValid() || !Json->TryGetObjectField(TEXT("payload"), PObj))
            {
                return;
            }

            const TSharedPtr<FJsonObject>& P = *PObj;

            FString MatchId, ServerAddr, Token;
            P->TryGetStringField(TEXT("matchId"), MatchId);
            P->TryGetStringField(TEXT("serverAddr"), ServerAddr);
            P->TryGetStringField(TEXT("token"), Token);

            if (auto* Sub = GameInstance->GetSubsystem<UGBClientMatchQueueSubsystem>()) 
            {
                Sub->HandleDungeonReady(MatchId, ServerAddr, Token);
            }
        });

    Socket->RegisterHandler(TEXT("JoinDungeon"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json)
        {
            const TSharedPtr<FJsonObject>* PObj = nullptr;
            if (!Json.IsValid() || !Json->TryGetObjectField(TEXT("payload"), PObj))
            {
                return;
            }
            const TSharedPtr<FJsonObject>& P = *PObj;

            FString Host, DungeonId, MatchId, TeamId, JoinToken;
            int32 Port = 0, TeamSize = 0;

            P->TryGetStringField(TEXT("host"), Host);
            P->TryGetNumberField(TEXT("port"), Port);
            P->TryGetStringField(TEXT("dungeonId"), DungeonId);
            P->TryGetStringField(TEXT("matchId"), MatchId);
            P->TryGetStringField(TEXT("teamId"), TeamId);
            P->TryGetNumberField(TEXT("teamSize"), TeamSize);
            P->TryGetStringField(TEXT("joinToken"), JoinToken);

            FDungeonJoinInfo Info;
            Info.Host = Host;
            Info.Port = Port;
            Info.DungeonId = DungeonId;
            Info.MatchId = MatchId;
            Info.TeamId = TeamId;
            Info.TeamSize = (TeamSize > 0) ? TeamSize : 3;
            Info.JoinToken = JoinToken;

            if (UGBDungeonJoinSubsystem* DungeonJoinSubsystem = GameInstance->GetSubsystem<UGBDungeonJoinSubsystem>())
            {
                // 월드를 얻는다. GameInstance->GetWorld()는 PIE/에디터 환경에서 nullptr일 수 있으니까
                // 더 안전하게는 DungeonJoinSubsystem->GetWorld()로 가져오자 (UGameInstanceSubsystem은 GetWorld() 가능)
                if (UWorld* World = DungeonJoinSubsystem->GetWorld())
                {
                    FTimerHandle DelayHandle;

                    World->GetTimerManager().SetTimer(
                        DelayHandle,
                        [DungeonJoinSubsystem, Info]()
                        {
                            // 딜레이 후 실제 접속 시도
                            DungeonJoinSubsystem->ConnectToDungeon(Info);
                        },
                        2.0f,   // DelaySeconds
                        false   // bLoop = false
                    );
                }
            }

            // UGBDungeonJoinSubsystem 사용 버전
           /* if (auto* DungeonJoinSubsystem = GameInstance->GetSubsystem<UGBDungeonJoinSubsystem>()) {
                FDungeonJoinInfo Info;
                Info.Host = Host;
                Info.Port = Port;
                Info.DungeonId = DungeonId;
                Info.MatchId = MatchId;
                Info.TeamId = TeamId;
                Info.TeamSize = (TeamSize > 0) ? TeamSize : 3;
                Info.JoinToken = JoinToken;
                DungeonJoinSubsystem->ConnectToDungeon(Info);
                return;
            }*/
        });

    Socket->RegisterHandler(TEXT("MatchFailed"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json)
        {
            const TSharedPtr<FJsonObject>* PObj = nullptr;
            if (!Json.IsValid() || !Json->TryGetObjectField(TEXT("payload"), PObj))
            {
                return;
            }

            const auto& P = *PObj;
            FString MatchId, Reason;
            P->TryGetStringField(TEXT("matchId"), MatchId);
            P->TryGetStringField(TEXT("reason"), Reason);

            if (auto* Sub = GameInstance->GetSubsystem<UGBClientMatchQueueSubsystem>()) 
            {
                Sub->HandleMatchFailed(MatchId, Reason);
            }
        });
}

void GBClientSocketResponseRegistrar::RegisterPartyHandlers(UGBClientSocketSubsystem* Socket, UGameInstance* GameInstance)
{
    Socket->RegisterHandler(TEXT("PartyInviteNotification"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json) {
            const TSharedPtr<FJsonObject>* PayloadObj = nullptr;

            Json->TryGetObjectField(TEXT("payload"), PayloadObj);
            const TSharedPtr<FJsonObject>& Payload = *PayloadObj;

            FString inviterName;
            Payload->TryGetStringField(TEXT("inviterName"), inviterName);


            UGBPopupManagerSubsystem* PopupManager = GameInstance->GetSubsystem<UGBPopupManagerSubsystem>();

            GB::ThreadingUtils::RunOnGameThread([WeakGI = TWeakObjectPtr<UGameInstance>(GameInstance), inviterName]()
                {
                    if (!WeakGI.IsValid())
                    {
                        return;
                    }
                    if (UGBPartyManager* PartyMgr = WeakGI->GetSubsystem<UGBPartyManager>())
                    {

                        PartyMgr->HandlePartyInviteReceived(inviterName);
                    }
                });
        });

    Socket->RegisterHandler(TEXT("PartyInfoNotification"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json) 
        {
            const TSharedPtr<FJsonObject>* PayloadObj = nullptr;

            Json->TryGetObjectField(TEXT("payload"), PayloadObj);
            const TSharedPtr<FJsonObject>& Payload = *PayloadObj;

            FPartyInfo NewPartyInfo;

            // 1. PartyId 가져오기
            int32 PartyId;
            if (Payload->TryGetNumberField(TEXT("partyId"), PartyId)) 
            {
                NewPartyInfo.PartyId = PartyId;
            }

            // 2. HostName 가져오기
            FString HostName;
            if (Payload->TryGetStringField(TEXT("hostName"), HostName)) 
            {
                NewPartyInfo.HostName = HostName;
            }

            // 3. Members 가져오기  
            const TArray<TSharedPtr<FJsonValue>>* MembersArray = nullptr;
            if (Payload->TryGetArrayField(TEXT("members"), MembersArray) && MembersArray) 
            {
                for (const TSharedPtr<FJsonValue>& MemberValue : *MembersArray) 
                {
                    const TSharedPtr<FJsonObject>* MemberObj = nullptr;
                    // 먼저 각 요소를 객체로 읽으려고 시도합니다.
                    if (MemberValue->TryGetObject(MemberObj) && MemberObj) {
                        FString MemberName;
                        // 객체에서 'userName' 필드를 문자열로 가져옵니다.
                        if ((*MemberObj)->TryGetStringField(TEXT("userName"), MemberName)) 
                        {
                            NewPartyInfo.Members.Add(MemberName);
                        }
                        else 
                        {
                            GB_BACKEND_LOG(Warning, TEXT("Could not find 'userName' field in a member object."));
                        }
                    }
                    else {
                        GB_BACKEND_LOG(Warning, TEXT("A member value is not a valid object."));
                    }
                }
            }
            else {
                GB_BACKEND_LOG(Warning, TEXT("Could not get 'members' array from payload."));
            }

            GB_BACKEND_LOG(Warning, TEXT("======== Received Party Info ============="));

            GB_BACKEND_LOG(Warning, TEXT("HostName : %s"), *NewPartyInfo.HostName);
            GB_BACKEND_LOG(Warning, TEXT("PartyID  : %d"), NewPartyInfo.PartyId);
            
            for (auto e : NewPartyInfo.Members)
            {
                GB_BACKEND_LOG(Warning, TEXT("Member  : %s"), *e);
            }

            GB_BACKEND_LOG(Warning, TEXT("=========================================="));

            GB::ThreadingUtils::RunOnGameThread([WeakGI = TWeakObjectPtr<UGameInstance>(GameInstance), NewPartyInfo]()
                {
                    if (!WeakGI.IsValid())
                    {
                        return;
                    }
                    if (UGBPartyManager* PartyMgr = WeakGI->GetSubsystem<UGBPartyManager>())
                    {
                        PartyMgr->HandlePartyInfoNotification(NewPartyInfo);
                    }

                });
        });

    Socket->RegisterHandler(TEXT("PartyJoinedByMe"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json) 
        {
            const TSharedPtr<FJsonObject>* PayloadObj = nullptr;

            Json->TryGetObjectField(TEXT("payload"), PayloadObj);
            const TSharedPtr<FJsonObject>& Payload = *PayloadObj;

            FPartyInfo NewPartyInfo;

            // 1. PartyId 가져오기
            int32 PartyId;
            if (Payload->TryGetNumberField(TEXT("partyId"), PartyId)) 
            {
                NewPartyInfo.PartyId = PartyId;
            }

            // 2. HostName 가져오기
            FString HostName;
            if (Payload->TryGetStringField(TEXT("hostName"), HostName)) 
            {
                NewPartyInfo.HostName = HostName;
            }

            // 3. Members 가져오기  
            const TArray<TSharedPtr<FJsonValue>>* MembersArray = nullptr;
            if (Payload->TryGetArrayField(TEXT("members"), MembersArray) && MembersArray) 
            {
                for (const TSharedPtr<FJsonValue>& MemberValue : *MembersArray) {
                    const TSharedPtr<FJsonObject>* MemberObj = nullptr;
                    // 먼저 각 요소를 객체로 읽으려고 시도합니다.
                    if (MemberValue->TryGetObject(MemberObj) && MemberObj) 
                    {
                        FString MemberName;
                        // 객체에서 'userName' 필드를 문자열로 가져옵니다.
                        if ((*MemberObj)->TryGetStringField(TEXT("userName"), MemberName)) 
                        {
                            NewPartyInfo.Members.Add(MemberName);
                        }
                        else {
                            GB_BACKEND_LOG(Warning, TEXT("Could not find 'userName' field in a member object."));
                        }
                    }
                    else {
                        GB_BACKEND_LOG(Warning, TEXT("A member value is not a valid object."));
                    }
                }
            }
            else {
                GB_BACKEND_LOG(Warning, TEXT("Could not get 'members' array from payload."));
            }

            GB_BACKEND_LOG(Warning, TEXT("======== Received Party Info ============="));

            GB_BACKEND_LOG(Warning, TEXT("HostName : %s"), *NewPartyInfo.HostName);
            GB_BACKEND_LOG(Warning, TEXT("PartyID  : %d"), NewPartyInfo.PartyId);

            for (auto e : NewPartyInfo.Members)
            {
                GB_BACKEND_LOG(Warning, TEXT("Member  : %s"), *e);
            }

            GB_BACKEND_LOG(Warning, TEXT("=========================================="));

            GB::ThreadingUtils::RunOnGameThread([WeakGI = TWeakObjectPtr<UGameInstance>(GameInstance), NewPartyInfo]()
                {
                    if (!WeakGI.IsValid())
                    {
                        return;
                    }
                    if (UGBPartyManager* PartyMgr = WeakGI->GetSubsystem<UGBPartyManager>())
                    {
                        PartyMgr->HandlePartyJoinedByMe(NewPartyInfo);
                    }

                });
        });

    Socket->RegisterHandler(TEXT("PartyMemberLeft"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json) {
            UE_LOG(LogTemp, Warning, TEXT("Received PartyMemberLeft Packet"));

            const TSharedPtr<FJsonObject>* PayloadObj = nullptr;

            Json->TryGetObjectField(TEXT("payload"), PayloadObj);
            const TSharedPtr<FJsonObject>& Payload = *PayloadObj;

            FString LeftPartyMemberName;
            Payload->TryGetStringField(TEXT("memberName"), LeftPartyMemberName);

            int32 PartyID;
            Payload->TryGetNumberField(TEXT("partyId"), PartyID);

            GB::ThreadingUtils::RunOnGameThread([WeakGI = TWeakObjectPtr<UGameInstance>(GameInstance), PartyID, LeftPartyMemberName]()
                {
                    if (!WeakGI.IsValid())
                    {
                        return;
                    }
                    if (UGBPartyManager* PartyMgr = WeakGI->GetSubsystem<UGBPartyManager>())
                    {

                        PartyMgr->HandlePartyMemberLeft(PartyID, LeftPartyMemberName);
                    }
                });
        });

    Socket->RegisterHandler(TEXT("PartyMemberJoined"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json) 
        {
            UE_LOG(LogTemp, Warning, TEXT("Received PartyMember Joined Packet"));

            const TSharedPtr<FJsonObject>* PayloadObj = nullptr;

            Json->TryGetObjectField(TEXT("payload"), PayloadObj);
            const TSharedPtr<FJsonObject>& Payload = *PayloadObj;

            FString JoinedPartyMemberName;
            Payload->TryGetStringField(TEXT("memberName"), JoinedPartyMemberName);

            int32 PartyID;
            Payload->TryGetNumberField(TEXT("partyId"), PartyID);

            GB::ThreadingUtils::RunOnGameThread([WeakGI = TWeakObjectPtr<UGameInstance>(GameInstance), PartyID, JoinedPartyMemberName]()
                {
                    if (!WeakGI.IsValid())
                    {
                        return;
                    }
                    if (UGBPartyManager* PartyMgr = WeakGI->GetSubsystem<UGBPartyManager>())
                    {

                        PartyMgr->HandlePartyMemberJoined(JoinedPartyMemberName);
                    }
                });
        });

    Socket->RegisterHandler(TEXT("PartyMemberKicked"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json) 
        {
            GB_BACKEND_LOG(Warning, TEXT("Received PartyMemberLeft Packet"));

            const TSharedPtr<FJsonObject>* PayloadObj = nullptr;

            Json->TryGetObjectField(TEXT("payload"), PayloadObj);
            const TSharedPtr<FJsonObject>& Payload = *PayloadObj;

            // PartyId를 가져다 쓰지는 않을 것 같음.. 
            int32 PartyId;
            Payload->TryGetNumberField(TEXT("partyID"), PartyId);

            FString KickedPartyMemberName;
            Payload->TryGetStringField(TEXT("memberName"), KickedPartyMemberName);

            UGBPartyManager* PartyManager = GameInstance->GetSubsystem<UGBPartyManager>();

            if (PartyManager)
            {
                PartyManager->HandlePartyMemberKickedReceived(KickedPartyMemberName);
            }
        });

    Socket->RegisterHandler(TEXT("SendPartyInviteResponse"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json) 
        {
            GB_BACKEND_LOG(Warning, TEXT("SendPartyInviteResponse Packet Received"));

            const TSharedPtr<FJsonObject>* PayloadObj = nullptr;

            Json->TryGetObjectField(TEXT("payload"), PayloadObj);
            const TSharedPtr<FJsonObject>& Payload = *PayloadObj;

            bool bSuccess = false;
            Payload->TryGetBoolField(TEXT("success"), bSuccess);

            const TOptional<FString> ErrorOpt = FGBJsonUtils::ReadOptionalString(Payload, TEXT("error"));
            const TOptional<FString> InviteeOpt = FGBJsonUtils::ReadOptionalString(Payload, TEXT("inviteeName"));

            UGBPartyManager* PartyManager = GameInstance->GetSubsystem<UGBPartyManager>();

            if (PartyManager)
            {
                PartyManager->HandleSendPartyInviteResponse(bSuccess, ErrorOpt, InviteeOpt);
            }
        });

    Socket->RegisterHandler(TEXT("PartyHostTransferred"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json) 
        {
            GB_BACKEND_LOG(Warning, TEXT("Received Party Host Transferred Packet"));

            const TSharedPtr<FJsonObject>* PayloadObj = nullptr;

            Json->TryGetObjectField(TEXT("payload"), PayloadObj);
            const TSharedPtr<FJsonObject>& Payload = *PayloadObj;

            FString OldHostName;
            Payload->TryGetStringField(TEXT("oldHostName"), OldHostName);

            FString NewHostName;
            Payload->TryGetStringField(TEXT("newHostName"), NewHostName);

            int32 PartyID;
            Payload->TryGetNumberField(TEXT("partyId"), PartyID);

            GB::ThreadingUtils::RunOnGameThread([WeakGI = TWeakObjectPtr<UGameInstance>(GameInstance), PartyID, OldHostName, NewHostName]()
                {
                    if (!WeakGI.IsValid())
                    {
                        return;
                    }
                    if (UGBPartyManager* PartyMgr = WeakGI->GetSubsystem<UGBPartyManager>())
                    {

                        PartyMgr->HandlePartyHostTransferred(PartyID, OldHostName, NewHostName);
                    }
                });

        });

    Socket->RegisterHandler(TEXT("PartyDisbanded"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json) {
            GB_BACKEND_LOG(Warning, TEXT("Party Disbanded Packet"));
        });
}

void GBClientSocketResponseRegistrar::RegisterGamePhaseHandlers(UGBClientSocketSubsystem* Socket, UGameInstance* GameInstance)
{
    Socket->RegisterHandler(TEXT("GamePhaseChanged"),
        [GameInstance](const TSharedPtr<FJsonObject>& Json) {
            GB_BACKEND_LOG(Warning, TEXT("GamePhaseChanged Packet Received!"));

            const TSharedPtr<FJsonObject>* PayloadObj = nullptr;

            Json->TryGetObjectField(TEXT("payload"), PayloadObj);
            const TSharedPtr<FJsonObject>& Payload = *PayloadObj;

            FString PreviousGameState;
            FString ChangedGameState;
            Payload->TryGetStringField(TEXT("previousGameState"), PreviousGameState);
            Payload->TryGetStringField(TEXT("changedGameState"), ChangedGameState);

            EGBClientGamePhase PrevGamePhase = EGBClientGamePhase::None;
            EGBClientGamePhase ChangedGamePhase = EGBClientGamePhase::None;

            UEnum* EnumPtr = StaticEnum<EGBClientGamePhase>();
            if (EnumPtr)
            {
                int64 Value = EnumPtr->GetValueByNameString(PreviousGameState);

                if (Value != INDEX_NONE)
                {
                    PrevGamePhase = static_cast<EGBClientGamePhase>(Value);
                }

                Value = EnumPtr->GetValueByNameString(ChangedGameState);

                if (Value != INDEX_NONE)
                {
                    ChangedGamePhase = static_cast<EGBClientGamePhase>(Value);
                }
            }

            UGBClientPhaseSubsystem* ClientPhaseSubsystem =  GameInstance->GetSubsystem<UGBClientPhaseSubsystem>();

            if (ClientPhaseSubsystem)
            {
                // 일단 true로 하드코딩.. false가 나올일이 있을까..?
                ClientPhaseSubsystem->HandleGamePhaseChanged(true, PrevGamePhase, ChangedGamePhase);
            }
        }); 
}
