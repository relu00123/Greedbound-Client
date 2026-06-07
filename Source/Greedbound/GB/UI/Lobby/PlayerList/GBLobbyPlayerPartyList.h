// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GB/Define/User/UserInfo.h"
#include "GB/Define/Party/GBPartyType.h"
#include "GBLobbyPlayerPartyList.generated.h"

class UGBLobbyPlayerEntryWidget;
class UGBPartyManager;
class UGBLobbyUserManager;

UCLASS()
class GREEDBOUND_API UGBLobbyPlayerPartyList : public UUserWidget
{
    GENERATED_BODY()
    
protected:
    virtual void                                        NativeConstruct() override;
    virtual void                                        NativeDestruct() override;

private:
    void                                                BindingDelegates();
    void                                                UnbindingDelegates();

    void                                                HandlePartyInfoUpdated(const FPartyInfo& PartyInfo); 
    void                                                HandlePartyMemberJoined(const FString& UserId);
    void                                                HandlePartyMemberLeft(const FString& UserId);
    void                                                HandlePartyMemberKicked(const FString& UserId);
    void                                                HandleUserInfoChanged(const FUserInfo& UserInfo);
    void                                                HandleUserClassChanged(const FString& UserName, EGBCharacterClassType ChangedClassType);
    void                                                HandleUserGamePhaseChanged(const FString& UserName, EGBClientGamePhase GamePhase); 
    void                                                HandlePartyHostTransfered(int32 PartyId, const FString& OldHostName, const FString& NewHostName);
    void                                                HandleKickedFromParty();
    void                                                HandleLeaveFromParty();

    void                                                AddOrUpdatePartyMemberVisual(const FString& UserId);
    void                                                FillSlot(UGBLobbyPlayerEntryWidget* TargetSlot, const FUserInfo& Info);
    void                                                ClearSlotByUser(const FString& UserId);
    void                                                ClearAllPartyMemberSlots();

    void                                                RebuildFromSnapshot(const FPartyInfo& PartyInfo);

    void                                                MarkPartyHost(const FString& HostUserId);
    void                                                CompactPartyMemberSlots(); 

    UGBLobbyPlayerEntryWidget*                          FindEmptySlot() const;
    
protected :
    // 위젯 바인딩
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGBLobbyPlayerEntryWidget>                   MySlot;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGBLobbyPlayerEntryWidget>                   FirstPartyMemberSlot;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGBLobbyPlayerEntryWidget>                   SecondPartyMemberSlot;

private:
    UPROPERTY()
    TArray<TObjectPtr<UGBLobbyPlayerEntryWidget>>           PartyMemberSlots;

    UPROPERTY() 
    TMap<FString, TObjectPtr<UGBLobbyPlayerEntryWidget>>    UserIdToSlot; // UserId → Slot (나 자신은 포함되어 있지 않음)

    FDelegateHandle                                         PartyInfoUpdatedHandle;
    FDelegateHandle                                         PartyJoinedHandle;
    FDelegateHandle                                         PartyMemberLeftHandle;
    FDelegateHandle                                         PartyLeaveHandle;
    FDelegateHandle                                         KickedFromPartyHandle;
    FDelegateHandle                                         PartyMemberKickedHandle;
    FDelegateHandle                                         UserInfoChangedHandle; 
    FDelegateHandle                                         UserClassChangedHandle;
    FDelegateHandle                                         UserGamePhaseChangedHandle;
    FDelegateHandle                                         PartyHostTransferedHandle;
};
