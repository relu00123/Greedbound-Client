// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Define/Character/GBClassType.h"
#include "GBLobbyWidget.generated.h"

class UGBLobbyTabButton;
class UTextBlock;
class UWidgetSwitcher;
class UBorder;
class UGBLobbyClassSelectWidget;
class UGBLobbyPlayerListWidget;

UENUM(BlueprintType)
enum class ELobbyTab : uint8
{
    MainMenu,
    Friends,
    Class
};

UCLASS()
class GREEDBOUND_API UGBLobbyWidget : public UUserWidget
{
    GENERATED_BODY()
    
public:
    virtual void    NativeConstruct() override;

private:
    UFUNCTION()
    void            OnClickMainMenu();

    UFUNCTION()
    void            OnClickFriends();

    UFUNCTION()
    void            OnClickClassSelect();

    void            SwitchToTab(ELobbyTab NewTab);

    void            UpdateTabFocus(UGBLobbyTabButton* FocusedTab);

    UFUNCTION()
    void            HandleConfirmClassChangeRequested(EGBCharacterClassType RequestedClass);

    UFUNCTION()
    void            HandleConfirmClassChangeResponse(bool bSuccess, EGBCharacterClassType  OldClass, EGBCharacterClassType NewClass);

    UFUNCTION() 
    void            HandleUserClassChanged(const FString& UserName, EGBCharacterClassType ChangedClassType);

private :
    // 위젯 바인딩
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UWidgetSwitcher>                     WidgetSwitcher;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGBLobbyTabButton>                   MainMenu;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGBLobbyTabButton>                   Friends;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGBLobbyTabButton>                   ClassSelect;

    // 친구 탭클릭시의 화면 위젯
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGBLobbyPlayerListWidget>            LobbyPlayerListPanel;

    // 클래스 선택 화면 위젯
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGBLobbyClassSelectWidget>           ClassSelectPanel;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UBorder>                             BackgroundBorder;

    // TabButton들을 모아 관리
    TArray<TObjectPtr<UGBLobbyTabButton>>           TabButtons; 

    UPROPERTY() 
    TMap<ELobbyTab, TObjectPtr<UGBLobbyTabButton>>  ButtonByTab;

    UPROPERTY() 
    TMap<ELobbyTab, int32>                          SwitchIndexByTab;

    UPROPERTY() 
    ELobbyTab                                       CurrentTab = ELobbyTab::MainMenu;
};
