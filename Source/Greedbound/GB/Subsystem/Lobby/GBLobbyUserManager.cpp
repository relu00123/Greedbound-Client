// Fill out your copyright notice in the Description page of Project Settings.

#include "GB/Subsystem/Lobby/GBLobbyUserManager.h"
#include "GB/Subsystem/Network/Client/GBClientSessionSubsystem.h"
#include "GB/Subsystem/Network/Client/GBClientSocketRequestHelper.h"
#include "Utils/GBThreadingUtils.h"

void UGBLobbyUserManager::InitUserList(const TArray<FUserInfo>& InList)
{
    CachedUserMap.Empty();

    for (const FUserInfo& Info : InList)
    {
        CachedUserMap.Add(Info.NickName, Info);
    }
}

void UGBLobbyUserManager::AddUser(const FUserInfo& NewUser)
{
    CachedUserMap.Add(NewUser.NickName, NewUser);
    OnUserAdded.Broadcast(NewUser);
    //OnUserAddedOrUpdated.Broadcast(NewUser);

    if (NewUser.NickName == GetGameInstance()->GetSubsystem<UGBClientSessionSubsystem>()->GetNickname())
    {
        SetMyUserInfo(NewUser);

        //MyUserInfo = NewUser;
    }
}

void UGBLobbyUserManager::RemoveUser(const FString& UserName)
{
    // 리펙토링 중인 부분
    /*if (CachedUserMap.Remove(UserName) > 0)
    {
        OnUserRemoved.Broadcast(UserName);
    }*/

    if (const FUserInfo* FoundUserInfo = CachedUserMap.Find(UserName))
    {
        // 복사본 생성
        FUserInfo RemovedInfo = *FoundUserInfo;
        RemovedInfo.bOnline = false;

        // 실제로 제거
        CachedUserMap.Remove(UserName);

        // Binding된 함수들 Broadcast
        OnUserRemoved.Broadcast(RemovedInfo);
    }
}

void UGBLobbyUserManager::ClearUserList()
{
    UE_LOG(LogTemp, Warning, TEXT("Clearing Cahced UserList"));
    CachedUserMap.Empty();
}

const TMap<FString, FUserInfo>& UGBLobbyUserManager::GetUserMap() const
{
    return CachedUserMap;
}

const FUserInfo* UGBLobbyUserManager::FindUser(const FString& UserName) const
{
    return CachedUserMap.Find(UserName);
}

const FUserInfo* UGBLobbyUserManager::GetMyUserInfo() const
{
    const FUserInfo* myuserInfo = CachedUserMap.Find(MyUserName);

    if (myuserInfo == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("MyUserInfo does not Exists [GBLobbyUserManager.cpp]"));
        return nullptr;
    }

    return myuserInfo;
}

const FString& UGBLobbyUserManager::GetMyUserName() const
{
    return MyUserName;
}

void UGBLobbyUserManager::SetMyUserInfo(const FUserInfo& MyInfo)
{
    const bool bWasReady = bMyUserReady;

    MyUserName = MyInfo.NickName;

    bMyUserReady = true;

    if (!bWasReady)
    {
        OnMyUserInfoReady.Broadcast(MyInfo);
    }
    else
    {
        OnMyUserInfoChanged.Broadcast(MyInfo);
    }

    // OnMyUserInfoChanged.Broadcast(MyInfo);
}

void UGBLobbyUserManager::SetClasss(const FString& UserName, EGBCharacterClassType ClassType)
{
    FUserInfo* FoundUser = CachedUserMap.Find(UserName);
    if (FoundUser == nullptr) 
    {
        return;
    }

    FoundUser->ClassType = ClassType;

    // Class 정보가 바뀌었음을 BroadCast해야 한다. 
    OnUserClassChanged.Broadcast(UserName, ClassType); // Lobby의 캐릭터들 변경 
}

void UGBLobbyUserManager::SetGamePhase(const FString& UserName, EGBClientGamePhase GamePhase)
{
    FUserInfo* FoundUser = CachedUserMap.Find(UserName);
    if (FoundUser == nullptr) 
    {
        return;
    }

    EGBClientGamePhase BeforeGamePhase = FoundUser->ClientGamePhase;

    FoundUser->ClientGamePhase = GamePhase;

    // 유저의 GamePhase정보가 바뀌었음을 Broadcast해야한다.
    OnUserGamePhaseChanged.Broadcast(UserName, GamePhase);
}

void UGBLobbyUserManager::RequestChangeClass(EGBCharacterClassType NewClass)
{
    /*if (MyUserInfo.ClassType == NewClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("Request Change Class Failed due to Smae Class request [GBLobbyuserManager.cpp]"))
        return;
    }*/

    bChangeClassPending = true;

    GBClientSocketRequestHelper::SendChangeClassRequest(NewClass);
}

void UGBLobbyUserManager::HandleChangeMyUserInfoResult(bool bSuccess, const FUserInfo& NewInfo)
{
    if (!bSuccess) 
    {
        return;
    }

    GB::ThreadingUtils::RunOnGameThread([this, NewInfo]()
        {
            SetMyUserInfo(NewInfo);
        });
}

void UGBLobbyUserManager::HandleChangeClassResponse(bool bSuccess, EGBCharacterClassType OldClass, EGBCharacterClassType NewClass)
{
    GB::ThreadingUtils::RunOnGameThread([this, bSuccess, OldClass, NewClass]()
        {
            bChangeClassPending = false;
            OnChangeClassResult.Broadcast(bSuccess, OldClass, NewClass);

            // 실험..
        });
}

void UGBLobbyUserManager::HandleUpdateUserInfo(FUserInfo userInfo)
{
    // UserInfo Map에서 Update
    // Delegate Broadcast
    //    Broadcast를 위해서 Lobby나 Party나 등에서 UserInfoChange를 Binding해두어야함. Lobby에서 일단 한번만 받고
    // Lobby에서 직접 전파하는 방식을 사용할지는 고민해봐야겠.. 
}
