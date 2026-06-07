// Fill out your copyright notice in the Description page of Project Settings.

#include "GBLobbyWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Subsystem/Network/Client/GBClientSessionSubsystem.h"
#include "Subsystem/Network/Client/GBClientHttpManagerSubsystem.h"
#include "GBLobbyTabButton.h"
#include "Components/Border.h"
#include "UI/Lobby/ClassSelect/GBLobbyClassSelectWidget.h"
#include "Subsystem/Network/Client/GBClientSocketRequestHelper.h"
#include "Subsystem/Lobby/GBLobbyUserManager.h"
#include "UI/Lobby/PlayerList/GBLobbyPlayerListWidget.h"


void UGBLobbyWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 배열 초기화 
    TabButtons = { MainMenu, Friends, ClassSelect };

    ButtonByTab =
    {
       { ELobbyTab::MainMenu,  MainMenu },
       { ELobbyTab::Friends,   Friends   },
       { ELobbyTab::Class,     ClassSelect }
    };

    SwitchIndexByTab =
    {
        { ELobbyTab::MainMenu,  0 },
        { ELobbyTab::Friends,   1 },
        { ELobbyTab::Class,     2 }
    };

    if (MainMenu)
        MainMenu->TabButton->OnClicked.AddDynamic(this, &UGBLobbyWidget::OnClickMainMenu);

    if (Friends)
        Friends->TabButton->OnClicked.AddDynamic(this, &UGBLobbyWidget::OnClickFriends);

    if (ClassSelect)
        ClassSelect->TabButton->OnClicked.AddDynamic(this, &UGBLobbyWidget::OnClickClassSelect);

    SwitchToTab(CurrentTab);

    if (ClassSelectPanel)
    {
        ClassSelectPanel->BindOnConfirmChangeRequested(this, GET_FUNCTION_NAME_CHECKED(UGBLobbyWidget, HandleConfirmClassChangeRequested));
    }

    if (UGBLobbyUserManager* LobbyUserManager = GetGameInstance()->GetSubsystem<UGBLobbyUserManager>())
    {
        LobbyUserManager->BindOnChangeClassResult(FOnChangeClassResult::FDelegate::CreateUObject(
            this, &UGBLobbyWidget::HandleConfirmClassChangeResponse
        ));

        LobbyUserManager->BindOnUserClassChanged(FOnUserClassChanged::FDelegate::CreateUObject(
            this, &UGBLobbyWidget::HandleUserClassChanged
        ));
    }
}

void UGBLobbyWidget::OnClickMainMenu()
{
    SwitchToTab(ELobbyTab::MainMenu);
}

void UGBLobbyWidget::OnClickFriends()
{
    SwitchToTab(ELobbyTab::Friends);
}

void UGBLobbyWidget::OnClickClassSelect()
{
    SwitchToTab(ELobbyTab::Class);
}

void UGBLobbyWidget::SwitchToTab(ELobbyTab NewTab)
{
    const ELobbyTab OldTab = CurrentTab;

    // Class 탭을 떠날때 TempSelect 정리 
    if (OldTab == ELobbyTab::Class && ClassSelectPanel)
    {
        ClassSelectPanel->ClearTempSelection();
    }

    CurrentTab = NewTab;

    if (const int32* Index = SwitchIndexByTab.Find(NewTab))
    {
        WidgetSwitcher->SetActiveWidgetIndex(*Index);
    }

    if (TObjectPtr<UGBLobbyTabButton>* Btn = ButtonByTab.Find(NewTab))
    {
        UpdateTabFocus(*Btn);
    }

    if (BackgroundBorder)
    {
        const bool bMain = (NewTab == ELobbyTab::MainMenu);
        BackgroundBorder->SetVisibility(bMain ? ESlateVisibility::Hidden
            : ESlateVisibility::Visible);
    }
}


void UGBLobbyWidget::UpdateTabFocus(UGBLobbyTabButton* FocusedTab)
{
    for (UGBLobbyTabButton* Tab : TabButtons)
    {
        if (!Tab) continue;

        const bool bIsFocused = (Tab == FocusedTab);
        Tab->SetFocusDecorationVisible(bIsFocused);
    }
}

void UGBLobbyWidget::HandleConfirmClassChangeRequested(EGBCharacterClassType RequestedClass)
{
    if (UGBLobbyUserManager* LobbyUserManager = GetGameInstance()->GetSubsystem<UGBLobbyUserManager>())
    {
        //FUserInfo MyUserInfo = *LobbyUserManager->GetMyUserInfo();

        const FUserInfo* MyUserInfo = LobbyUserManager->GetMyUserInfo();

        if (MyUserInfo == nullptr)
        {
            UE_LOG(LogTemp, Warning, TEXT("MyUserInfo is null[GBLobbyWidget.cpp]"));
            return;
        }


        // 같은 클래스면 전송 불필요
        if (MyUserInfo->ClassType == RequestedClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("HandleConfirmClassChange Request Failed Due to SameClass [GBLobbyWidget.cpp]"));
            UE_LOG(LogTemp, Warning, TEXT("MyUserInfo.Class : %s "), *UEnum::GetValueAsString(MyUserInfo->ClassType));
            UE_LOG(LogTemp, Warning, TEXT("Requested Class : %s"), *UEnum::GetValueAsString(RequestedClass));
            if (ClassSelectPanel) ClassSelectPanel->SetClassSelectPending(false);
            return;
        }

        LobbyUserManager->RequestChangeClass(RequestedClass);
    }
    else
    {
        // 매니저가 없으면 안전하게 리턴 (check 대신 ensure 권장)
        ensureMsgf(false, TEXT("UGBLobbyUserManager not found"));
    }
}

void UGBLobbyWidget::HandleConfirmClassChangeResponse(bool bSuccess, EGBCharacterClassType OldClass, EGBCharacterClassType NewClass)
{
    if (ClassSelectPanel)
    {
        ClassSelectPanel->SetClassSelectPending(false);
        ClassSelectPanel->ClearTempSelection();
    }
}

void UGBLobbyWidget::HandleUserClassChanged(const FString& UserName, EGBCharacterClassType ChangedClassType)
{
    if (UGBLobbyUserManager* LobbyUserManager = GetGameInstance()->GetSubsystem<UGBLobbyUserManager>())
    {
        FString MyName = LobbyUserManager->GetMyUserInfo()->NickName;

        if (MyName == UserName)
        {
            ClassSelectPanel->SelectClass(ChangedClassType);
        }

    }
}


