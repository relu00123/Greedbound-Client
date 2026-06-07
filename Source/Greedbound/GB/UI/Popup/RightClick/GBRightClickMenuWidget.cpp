// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Popup/RightClick/GBRightClickMenuWidget.h"
#include "UI/Popup/RightClick/GBRightClickMenuEntryWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "UI/Lobby/GBLobbyHUD.h"

void UGBRightClickMenuWidget::SetupEntries(const TArray<FRightClickMenuEntry>& Entries, const FString& TargetNickName)
{
    if (!MenuContainer || !EntryWidgetClass) return;

    MenuContainer->ClearChildren();

    for (const FRightClickMenuEntry& Entry : Entries)
    {
        UGBRightClickMenuEntryWidget* EntryWidget = CreateWidget<UGBRightClickMenuEntryWidget>(GetWorld(), EntryWidgetClass);
        if (!EntryWidget) continue;

        EntryWidget->SetEntry(Entry);
        EntryWidget->OnMenuEntryClicked.AddDynamic(this, &UGBRightClickMenuWidget::HandleMenuEntryClicked);

        MenuContainer->AddChild(EntryWidget);
    }
}

void UGBRightClickMenuWidget::HandleMenuEntryClicked(ERightClickMenuAction Action)
{
    OnMenuActionSelected.Broadcast(Action);
    // RemoveFromParent(); // 자동 닫기.. 이부분은 PopupManager에서 해야하지 않나? 
}
