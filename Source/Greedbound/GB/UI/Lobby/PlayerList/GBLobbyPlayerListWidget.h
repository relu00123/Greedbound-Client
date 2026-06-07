// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Define/UI/GBLobbyUITypes.h"
#include "GBLobbyPlayerListWidget.generated.h"

class UWidgetSwitcher; 
class UGBLobbyTabButton;
class UGBLobbyPlayerGrid;
class UGBLobbyPlayerPartyList;

UCLASS()
class GREEDBOUND_API UGBLobbyPlayerListWidget : public UUserWidget
{
    GENERATED_BODY()
    
public :
    FORCEINLINE EGBLobbyPlayerListType      GetFocusedLobbyPlayerListType() { return EFocusedLobbyPlayerListType; }

protected :
    virtual void                            NativeConstruct() override; 
    virtual void                            NativeDestruct() override;

private:
    UFUNCTION()
    void                                    OnClickLobbyBtn();

    UFUNCTION()
    void                                    OnClickFriendBtn();

    UFUNCTION()
    void                                    OnClickReceivedFriendRequestBtn();

    UFUNCTION()
    void                                    OnClickSentFriendRequestBtn();

    void                                    UpdateTabFocus(UGBLobbyTabButton* FocusedTab);

private :
    EGBLobbyPlayerListType                  EFocusedLobbyPlayerListType; 

    // 위젯 바인딩 
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UWidgetSwitcher>             PlayerGridWidgetSwitcher;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGBLobbyPlayerGrid>          LobbyGrid;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGBLobbyPlayerGrid>          FriendGrid;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGBLobbyPlayerGrid>          ReceivedFriendRequestGrid;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGBLobbyPlayerGrid>          SentFriendRequestGrid;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGBLobbyTabButton>           LobbyBtn;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGBLobbyTabButton>           FriendBtn;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGBLobbyTabButton>           ReceivedFriendRequestBtn;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGBLobbyTabButton>           SentFriendRequestBtn;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGBLobbyPlayerPartyList>     PartyList;

    // TabButton들을 모아 관리
    TArray<TObjectPtr<UGBLobbyTabButton>>   TabButtons;


    FDelegateHandle                         OnUserAddedHandleForLobbyGrid;

    FDelegateHandle                         OnUserRemovedHandleForLobbyGrid; 

    FDelegateHandle                         OnSentFriendRequestAddedHandle;

    FDelegateHandle                         OnSentFriendRequestRemovedHandle;

    FDelegateHandle                         OnUserAddedHandleForSentFriendRequestGrid;

    FDelegateHandle                         OnUserRemovedHandleForSentFriendRequestGrid;

    FDelegateHandle                         OnReceivedFriendRequestAddedHandle;

    FDelegateHandle                         OnReceivedFriendRequestRemovedHandle;

    FDelegateHandle                         OnUserAddedHandleForReceivedFriendRequestGrid;

    FDelegateHandle                         OnUserRemovedHandleForReceivedFriendRequestGrid;

    FDelegateHandle                         OnFriendAddedHandle;

    FDelegateHandle                         OnFriendRemovedHandle; 

    FDelegateHandle                         OnUserAddedForFriendGrid; 

    FDelegateHandle                         OnUserRemovedForFriendGrid;




 
};
