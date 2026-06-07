// Fill out your copyright notice in the Description page of Project Settings.


#include "GBLobbyPlayerListWidget.h"
#include "UI/Lobby/GBLobbyTabButton.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "UI/Lobby/PlayerList/GBLobbyPlayerGrid.h"
#include "Subsystem/Lobby/GBLobbyUserManager.h"
#include "Subsystem/Social/GBFriendManager.h"
#include "UI/Lobby/PlayerList/GBLobbyPlayerPartyList.h"

void UGBLobbyPlayerListWidget::NativeConstruct()
{
    Super::NativeConstruct();
    // 배열 초기화 
    TabButtons = { LobbyBtn, FriendBtn, ReceivedFriendRequestBtn, SentFriendRequestBtn };

    if (LobbyBtn)
        LobbyBtn->TabButton->OnClicked.AddDynamic(this, &UGBLobbyPlayerListWidget::OnClickLobbyBtn);

    if (FriendBtn)
        FriendBtn->TabButton->OnClicked.AddDynamic(this, &UGBLobbyPlayerListWidget::OnClickFriendBtn);

    if (ReceivedFriendRequestBtn)
        ReceivedFriendRequestBtn->TabButton->OnClicked.AddDynamic(this, &UGBLobbyPlayerListWidget::OnClickReceivedFriendRequestBtn);

    if (SentFriendRequestBtn)
        SentFriendRequestBtn->TabButton->OnClicked.AddDynamic(this, &UGBLobbyPlayerListWidget::OnClickSentFriendRequestBtn);

    UGBFriendManager* FriendManager = GetGameInstance()->GetSubsystem<UGBFriendManager>();
    check(FriendManager);

    UGBLobbyUserManager* LobbyUserManager = GetGameInstance()->GetSubsystem<UGBLobbyUserManager>();
    check(LobbyUserManager);

    if (LobbyGrid)
    {
        TMap<FString, FUserInfo> usermap = LobbyUserManager->GetUserMap();

        TArray<FUserInfo> userList;
        usermap.GenerateValueArray(userList); // 값(value)만 TArray로 추출

        LobbyGrid->Init(userList, EGBLobbyPlayerListType::Lobby); // Init 함수는 TArray<FUserInfo> 를 받음


        // !! (1)
        OnUserAddedHandleForLobbyGrid = LobbyUserManager->BindOnUserAdded(
            TDelegate<void(const FUserInfo&)>::CreateLambda(
                [this](const FUserInfo& Info)
                {
                    if (LobbyGrid)
                        LobbyGrid->OnUserAdded(Info);
                }
            ));

        // !! (2)
        OnUserRemovedHandleForLobbyGrid =  LobbyUserManager->BindOnUserRemoved(
            TDelegate<void(const FUserInfo&)>::CreateLambda(
                [this](const FUserInfo& UserInfo)
                {
                    if (LobbyGrid)
                        LobbyGrid->OnUserRemoved(UserInfo);
                }
            ));
    }

    if (SentFriendRequestGrid)
    {
        TSet<FString>  SentFriendRequestMap = FriendManager->GetSentFrinedRequests();
        TArray<FUserInfo>  SentFriendRequestList;

        for (const FString& name : SentFriendRequestMap)
        {
            FUserInfo userInfo;
            userInfo.NickName = name;

            const FUserInfo* FoundUserInfo = LobbyUserManager->FindUser(name);
            if (FoundUserInfo != nullptr)
            {
                userInfo.ClassType = FoundUserInfo->ClassType;
                userInfo.bOnline = true;
            }
            else
            {
                int32 RandomIndex = FMath::RandRange(0, static_cast<int32>(EGBCharacterClassType::Max) - 1);
                userInfo.ClassType = static_cast<EGBCharacterClassType>(RandomIndex);
                userInfo.bOnline = false;
            }

            SentFriendRequestList.Add(userInfo);
        }

        SentFriendRequestGrid->Init(SentFriendRequestList, EGBLobbyPlayerListType::SentFriendRequest);

        // !! (11)
        // FriendManager 델리게이트 바인딩
        OnSentFriendRequestAddedHandle = FriendManager->BindOnSentFriendRequestAdded(
            TDelegate<void(const FString&)>::CreateLambda([this, LobbyUserManager](const FString& TargetUserName)
                {
                    UE_LOG(LogTemp, Warning, TEXT("BindOnSentFriendRequestAdded Called!"));
                    if (!SentFriendRequestGrid || !LobbyUserManager) return;

                    const FUserInfo* FoundUser = LobbyUserManager->FindUser(TargetUserName);

                    if (FoundUser)
                    {
                        SentFriendRequestGrid->OnUserAdded(*FoundUser);
                    }

                    else
                    {
                        FUserInfo userInfo;
                        userInfo.NickName = TargetUserName;
                        int32 RandomIndex = FMath::RandRange(0, static_cast<int32>(EGBCharacterClassType::Max) - 1);
                        userInfo.ClassType = static_cast<EGBCharacterClassType>(RandomIndex);
                        userInfo.bOnline = false;
                        SentFriendRequestGrid->OnUserAdded(userInfo);
                    }
                })
        );

        // !! (12)
        OnSentFriendRequestRemovedHandle = FriendManager->BindOnSentFriendRequestRemoved(
            TDelegate<void(const FString&)>::CreateLambda([this, LobbyUserManager](const FString& TargetUserName)
                {
                    if (!SentFriendRequestGrid) return;

                    SentFriendRequestGrid->OnUserRemoved(TargetUserName);
                })
        );

        // !! (13)
        OnUserAddedHandleForSentFriendRequestGrid = LobbyUserManager->BindOnUserAdded(
            TDelegate<void(const FUserInfo&)>::CreateLambda([this](const FUserInfo& Info)
                {
                    if (SentFriendRequestGrid)
                        SentFriendRequestGrid->OnUserUpdated(Info);
                }
            ));

        // !! (14)
        OnUserRemovedHandleForSentFriendRequestGrid = LobbyUserManager->BindOnUserRemoved(
            TDelegate<void(const FUserInfo&)>::CreateLambda([this](const FUserInfo& Info)
                {
                    if (SentFriendRequestGrid)
                        SentFriendRequestGrid->OnUserUpdated(Info);
                }
            ));
    }

    if (ReceivedFriendRequestGrid)
    {
        TSet<FString>  ReceivedFriendRequestMap = FriendManager->GetReceivedFriendRequests();
        TArray<FUserInfo>  ReceivedFriendRequestList;

        for (const FString& name : ReceivedFriendRequestMap)
        {
            FUserInfo userInfo;
            userInfo.NickName = name;

            const FUserInfo* FoundUserInfo = LobbyUserManager->FindUser(name);
            if (FoundUserInfo != nullptr)
            {
                userInfo.ClassType = FoundUserInfo->ClassType;
                userInfo.bOnline = true;
            }
            else
            {
                int32 RandomIndex = FMath::RandRange(0, static_cast<int32>(EGBCharacterClassType::Max) - 1);
                userInfo.ClassType = static_cast<EGBCharacterClassType>(RandomIndex);
                userInfo.bOnline = false;
            }

            ReceivedFriendRequestList.Add(userInfo);
        }

        ReceivedFriendRequestGrid->Init(ReceivedFriendRequestList, EGBLobbyPlayerListType::ReceivedFriendRequest);

        // !! (3)
        // FriendManager 델리게이트 바인딩
        OnReceivedFriendRequestAddedHandle = FriendManager->BindOnReceivedFriendRequestAdded(
            TDelegate<void(const FString&)>::CreateLambda([this, LobbyUserManager](const FString& TargetUserName)
                {
                    UE_LOG(LogTemp, Warning, TEXT("BindOnReceivedFriendRequestAdded Called!"));

                    if (!ReceivedFriendRequestGrid || !LobbyUserManager) return;

                    const FUserInfo* FoundUser = LobbyUserManager->FindUser(TargetUserName);

                    if (FoundUser)
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Found User!"));
                        ReceivedFriendRequestGrid->OnUserAdded(*FoundUser);
                    }

                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Did not Find User!"));
                        FUserInfo userInfo;
                        userInfo.NickName = TargetUserName;
                        int32 RandomIndex = FMath::RandRange(0, static_cast<int32>(EGBCharacterClassType::Max) - 1);
                        userInfo.ClassType = static_cast<EGBCharacterClassType>(RandomIndex);
                        userInfo.bOnline = false;
                        ReceivedFriendRequestGrid->OnUserAdded(userInfo);
                    }
                })
        );

        // !! (4)
        OnReceivedFriendRequestRemovedHandle = FriendManager->BindOnReceivedFriendRequestRemoved(
            TDelegate<void(const FString&)>::CreateLambda([this, LobbyUserManager](const FString& TargetUserName)
                {
                    if (!ReceivedFriendRequestGrid) return;

                    ReceivedFriendRequestGrid->OnUserRemoved(TargetUserName);
                })
        );

        // !! (5)
        // LobbyManager 델리게이터 바인딩
        OnUserAddedHandleForReceivedFriendRequestGrid = LobbyUserManager->BindOnUserAdded(
            TDelegate<void(const FUserInfo&)>::CreateLambda([this](const FUserInfo& Info)
                {
                    if (ReceivedFriendRequestGrid)
                        ReceivedFriendRequestGrid->OnUserUpdated(Info);
                }
            ));

        // !! (6)
        OnUserRemovedHandleForReceivedFriendRequestGrid = LobbyUserManager->BindOnUserRemoved(
            TDelegate<void(const FUserInfo&)>::CreateLambda([this](const FUserInfo& Info)
                {
                    if (ReceivedFriendRequestGrid)
                        ReceivedFriendRequestGrid->OnUserUpdated(Info);
                }
            ));
    }

    if (FriendGrid)
    {

        TSet<FString>  FriendListMap = FriendManager->GetFriendList();
        TArray<FUserInfo>  FriendList;

        for (const FString& name : FriendListMap)
        {
            FUserInfo userInfo;
            userInfo.NickName = name;

            const FUserInfo* FoundUserInfo = LobbyUserManager->FindUser(name);
            if (FoundUserInfo != nullptr)
            {
                userInfo.ClassType = FoundUserInfo->ClassType;
                userInfo.bOnline = true;
            }
            else
            {
                int32 RandomIndex = FMath::RandRange(0, static_cast<int32>(EGBCharacterClassType::Max) - 1);
                userInfo.ClassType = static_cast<EGBCharacterClassType>(RandomIndex);
                userInfo.bOnline = false;
            }

            FriendList.Add(userInfo);
        }

        FriendGrid->Init(FriendList, EGBLobbyPlayerListType::Friend);

        // !! (7)
        // FriendManager 델리게이트 바인딩
        OnFriendAddedHandle = FriendManager->BindOnFriendAdded(
            TDelegate<void(const FString&)>::CreateLambda([this, LobbyUserManager](const FString& TargetUserName)
                {
                    if (!FriendGrid || !LobbyUserManager) return;

                    const FUserInfo* FoundUser = LobbyUserManager->FindUser(TargetUserName);

                    if (FoundUser)
                    {
                        UE_LOG(LogTemp, Warning, TEXT("BindOnFrinedAdded Called!"));
                        FriendGrid->OnUserAdded(*FoundUser);
                    }

                    else
                    {
                        FUserInfo userInfo;
                        userInfo.NickName = TargetUserName;
                        int32 RandomIndex = FMath::RandRange(0, static_cast<int32>(EGBCharacterClassType::Max) - 1);
                        userInfo.ClassType = static_cast<EGBCharacterClassType>(RandomIndex);
                        userInfo.bOnline = false;
                        FriendGrid->OnUserAdded(userInfo);
                    }
                })
        );

        // !! (8)
        OnFriendRemovedHandle = FriendManager->BindOnFriendRemoved(
            TDelegate<void(const FString&)>::CreateLambda([this, LobbyUserManager](const FString& TargetUserName)
                {
                    if (!FriendGrid) return;

                    FriendGrid->OnUserRemoved(TargetUserName);
                })
        );

        // !! (9)
        // LobbyManager 델리게이트 바인딩
        OnUserAddedForFriendGrid =LobbyUserManager->BindOnUserAdded(
            TDelegate<void(const FUserInfo&)>::CreateLambda(
                [this](const FUserInfo& Info)
                {
                    if (FriendGrid)
                        FriendGrid->OnUserUpdated(Info);
                }
            )
        );

        // !! (10)
        // 오로지 Offline으로만 바꾸도록 할 것임. 
        OnUserRemovedForFriendGrid = LobbyUserManager->BindOnUserRemoved(
            TDelegate<void(const FUserInfo&)>::CreateLambda(
                [this](const FUserInfo& UserInfo)
                {
                    if (FriendGrid)
                    {
                        FriendGrid->OnUserUpdated(UserInfo);
                    }

                }
            )
        );
    }

    // Class변경 등록해야한다. 

    UpdateTabFocus(LobbyBtn);
}

void UGBLobbyPlayerListWidget::NativeDestruct()
{
    UGBLobbyUserManager* LobbyUserManager = GetGameInstance()->GetSubsystem<UGBLobbyUserManager>();

    if (LobbyUserManager)
    {
        // !! (1)
        LobbyUserManager->UnbindOnUserAdded(OnUserAddedHandleForLobbyGrid);

        // !! (2)
        LobbyUserManager->UnbindOnUserRemoved(OnUserRemovedHandleForLobbyGrid);

        // !! (5)
        LobbyUserManager->UnbindOnUserAdded(OnUserAddedHandleForReceivedFriendRequestGrid);

        // !! (6)
        LobbyUserManager->UnbindOnUserRemoved(OnUserRemovedHandleForReceivedFriendRequestGrid);

        // !! (9)
        LobbyUserManager->UnbindOnUserAdded(OnUserAddedForFriendGrid);

        // !! (13)
        LobbyUserManager->UnbindOnUserAdded(OnUserAddedHandleForSentFriendRequestGrid);

        // !! (10)
        LobbyUserManager->UnbindOnUserRemoved(OnUserRemovedForFriendGrid);

        // !! (14)
        LobbyUserManager->UnbindOnUserRemoved(OnUserRemovedHandleForSentFriendRequestGrid);
    }

    UGBFriendManager* FriendManager = GetGameInstance()->GetSubsystem<UGBFriendManager>();

    if (FriendManager)
    {
        // !! (3)
        FriendManager->UnbindOnReceivedFriendRequestAdded(OnReceivedFriendRequestAddedHandle);

        // !! (4) 
        FriendManager->UnbindOnReceivedFriendRequestRemoved(OnReceivedFriendRequestRemovedHandle);

        // !! (7)
        FriendManager->UnbindOnFriendAdded(OnFriendAddedHandle);

        // !! (8)
        FriendManager->UnbindOnFriendRemoved(OnFriendRemovedHandle);

        // !! (11)
        FriendManager->UnbindOnSentFriendRequestAdded(OnSentFriendRequestAddedHandle);

        // !! (12)
        FriendManager->UnbindOnSentFriendRequestRemoved(OnSentFriendRequestRemovedHandle);
    }

    Super::NativeDestruct();
}

void UGBLobbyPlayerListWidget::OnClickLobbyBtn()
{
    if (PlayerGridWidgetSwitcher)
    {
        PlayerGridWidgetSwitcher->SetActiveWidgetIndex(0);
    }

    EFocusedLobbyPlayerListType = EGBLobbyPlayerListType::Lobby;

    UpdateTabFocus(LobbyBtn);
}

void UGBLobbyPlayerListWidget::OnClickFriendBtn()
{
    if (PlayerGridWidgetSwitcher)
    {
        PlayerGridWidgetSwitcher->SetActiveWidgetIndex(1);
    }

    EFocusedLobbyPlayerListType = EGBLobbyPlayerListType::Friend;


    UpdateTabFocus(FriendBtn);
}

void UGBLobbyPlayerListWidget::OnClickReceivedFriendRequestBtn()
{
    if (PlayerGridWidgetSwitcher)
    {
        PlayerGridWidgetSwitcher->SetActiveWidgetIndex(3);
    }

    EFocusedLobbyPlayerListType = EGBLobbyPlayerListType::ReceivedFriendRequest;


    UpdateTabFocus(ReceivedFriendRequestBtn);
}

void UGBLobbyPlayerListWidget::OnClickSentFriendRequestBtn()
{
    if (PlayerGridWidgetSwitcher)
    {
        PlayerGridWidgetSwitcher->SetActiveWidgetIndex(2);
    }

    EFocusedLobbyPlayerListType = EGBLobbyPlayerListType::SentFriendRequest;

    UpdateTabFocus(SentFriendRequestBtn);
}

void UGBLobbyPlayerListWidget::UpdateTabFocus(UGBLobbyTabButton* FocusedTab)
{
    for (UGBLobbyTabButton* Tab : TabButtons)
    {
        if (!Tab)
        {
            continue;
        }

        const bool bIsFocused = (Tab == FocusedTab);
        Tab->SetFocusDecorationVisible(bIsFocused);
    }
}
