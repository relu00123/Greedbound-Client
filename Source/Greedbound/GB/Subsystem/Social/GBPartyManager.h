// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Define/User/UserInfo.h"
#include "Define/Party/GBPartyType.h"
#include "GBPartyManager.generated.h"


// @description 클라이언트 파티 정보를 관리하는 서브시스템 클래스
UCLASS()
class GREEDBOUND_API UGBPartyManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Handle
    UFUNCTION(BlueprintCallable, Category = "Party")
    void                            HandlePartyInfoNotification(const FPartyInfo& PartyInfo);

    UFUNCTION(BlueprintCallable, Category = "Party")
    void                            HandlePartyJoinedByMe(const FPartyInfo& PartyInfo);

    UFUNCTION(BlueprintCallable, Category = "Party")
    void                            HandlePartyInviteReceived(const FString& inviterName);

    UFUNCTION(BlueprintCallable, Category = "Party")
    void                            HandlePartyMemberKickedReceived(const FString& KickedMemberName);

    UFUNCTION(BlueprintCallable, Category = "Party")
    void                            HandleSendPartyInviteResponse(bool bSuccess, TOptional<FString> Error, TOptional<FString> InviteeName);

    UFUNCTION(BlueprintCallable, Category = "Party")
    void                            HandlePartyMemberJoined(const FString& NewMemberName);

    UFUNCTION(BlueprintCallable, Category = "Party")
    void                            HandlePartyMemberLeft(int32 PartyId, const FString& MemberName);

    UFUNCTION(BlueprintCallable, Category = "Party")
    void                            HandlePartyHostTransferred(int32 PartyId, const FString& OldHostName, const FString& NewHostName);

    // Send 
    void                            SendInviteToPartyRequest(const FString& TargetNickName);

    void                            SendKickFromPartyRequest(const FString& TargetNickName);

    void                            SendPartyHostTransferRequest(const FString& TargetNickName);

    void                            SendLeaveFromPartyRequest(const FString& MyNickName);
    
    bool                            IsInParty(const FString& UserId) const;

    bool                            IsPartyHost() const ;

    // 현재 표시중인 파티 초대에 대한 의사 결정 (UI에서 호출한다)
    void                            AcceptCurrentInvite();
    void                            DeclineCurrentInvite();

    UFUNCTION(BlueprintPure, Category = "Party")
    FORCEINLINE FPartyInfo          GetCurrentPartyInfo()  { return CurrentPartyInfo; }

    // Bindings
    FORCEINLINE FDelegateHandle     BindOnPartyInfoUpdated(const FOnPartyInfoUpdated::FDelegate& Delegate) { return OnPartyInfoUpdated.Add(Delegate); }
    FORCEINLINE void                UnbindOnPartyInfoUpdated(FDelegateHandle Handle) { OnPartyInfoUpdated.Remove(Handle); }

    FORCEINLINE FDelegateHandle     BindOnPartyJoinedByMe(const FOnPartyJoinedByMe::FDelegate& Delegate) { return OnPartyJoinedByMe.Add(Delegate); }
    FORCEINLINE void                UnbindOnPartyJoinedByMe(FDelegateHandle Handle) { OnPartyJoinedByMe.Remove(Handle); }

    FORCEINLINE FDelegateHandle     BindOnMyPartyRoleChanged(const FOnMyPartyRoleChanged::FDelegate& Delegate) { return OnMyPartyRoleChanged.Add(Delegate); }
    FORCEINLINE void                UnbindOnMyPartyRoleChanged(FDelegateHandle Handle) { OnMyPartyRoleChanged.Remove(Handle); }

    FORCEINLINE FDelegateHandle     BindOnPartyMemberJoined(const FOnPartyMemberJoined::FDelegate& Delegate) { return OnPartyMemberJoined.Add(Delegate); }
    FORCEINLINE void                UnbindOnPartyMemberJoined(FDelegateHandle Handle) { OnPartyMemberJoined.Remove(Handle); }

    FORCEINLINE FDelegateHandle     BindOnPartyMemberLeft(const FOnPartyMemberLeft::FDelegate& Delegate) { return OnPartyMemberLeft.Add(Delegate); }
    FORCEINLINE void                UnbindOnPartyMemberLeft(FDelegateHandle Handle) { OnPartyMemberLeft.Remove(Handle); }

    FORCEINLINE FDelegateHandle     BindOnLeaveFromParty(const FOnLeaveFromParty::FDelegate& Delegate) { return OnLeaveFromParty.Add(Delegate); }
    FORCEINLINE void                UnbindOnLeaveFromParty(FDelegateHandle Handle) { OnLeaveFromParty.Remove(Handle); }

    FORCEINLINE FDelegateHandle     BindOnKickedFromParty(const FOnKickedFromParty::FDelegate& Delegate) { return OnKickedFromParty.Add(Delegate); }
    FORCEINLINE void                UnbindOnKickedFromParty(FDelegateHandle Handle) { OnKickedFromParty.Remove(Handle); }

    FORCEINLINE FDelegateHandle     BindOnPartyMemberKicked(const FOnPartyMemberKicked::FDelegate& Delegate) { return OnPartyMemberKicked.Add(Delegate); }
    FORCEINLINE void                UnbindOnPartyMemberKicked(FDelegateHandle Handle) { OnPartyMemberKicked.Remove(Handle); }

    FORCEINLINE FDelegateHandle     BindOnHostTransferred(const FOnHostTransferred::FDelegate& Delegate) { return OnHostTransferred.Add(Delegate); }
    FORCEINLINE void                UnbindOnHostTransferred(FDelegateHandle Handle) { OnHostTransferred.Remove(Handle); }

    FORCEINLINE FDelegateHandle     BindOnActivePartyInviteChanged(const FOnActivePartyInviteChanged::FDelegate& Delegate) { return OnActivePartyInviteChanged.Add(Delegate);}
    FORCEINLINE void                RemoveOnActivePartyInviteChanged(FDelegateHandle Handle) { OnActivePartyInviteChanged.Remove(Handle); }

    void                            HandleOnUserRemoved(const FUserInfo& UserInfo);

    FORCEINLINE EPartyRole          GetMyPartyRole() { return MyPartyRoleCached; }

protected:
    // @brief 서브시스템이 초기화될 때 호출됩니다.
    virtual void                    Initialize(FSubsystemCollectionBase& Collection) override;

    // @brief 서브시스템이 종료될 때 호출됩니다.
    virtual void                    Deinitialize() override;


private:
    // void                            OnMyUserInfoReady_Adapter(const FUserInfo& My); 

    // 큐에서 다음 초대를 꺼내 UI에 표시하도록 알림
    void                            TryPresentNextInvite();

    void                            BindingDelegates();

    void                            UnbindingDelegates();

    EPartyRole                      ComputeMyPartyRole() const;

    void                            UpdateMyPartyRoleAndBroadCastIfChanged();


private:
    // 현재 파티 정보
    UPROPERTY()
    FPartyInfo                      CurrentPartyInfo;

    // 나의 PartyRole Cache
    EPartyRole                      MyPartyRoleCached; 


    // FIFO Queue
    UPROPERTY()
    TArray<FPartyInvite>            PendingInvites;

    // 현재 팝업이 떠있는지 여부
    bool                            bInvitePopupActive = false;

    

    // @description 파티 정보 변경을 UI에 알리기 위한 델리게이트
    FOnPartyInfoUpdated             OnPartyInfoUpdated;

    FOnPartyJoinedByMe              OnPartyJoinedByMe;

    FOnMyPartyRoleChanged           OnMyPartyRoleChanged;

    FOnPartyMemberJoined            OnPartyMemberJoined;

    FOnPartyMemberLeft              OnPartyMemberLeft;

    FOnLeaveFromParty               OnLeaveFromParty;

    FOnKickedFromParty              OnKickedFromParty;

    FOnPartyMemberKicked            OnPartyMemberKicked;

    FOnHostTransferred              OnHostTransferred;

    FDelegateHandle                 MyUserInfoReadyHandle;

    FOnActivePartyInviteChanged     OnActivePartyInviteChanged;
};

 
