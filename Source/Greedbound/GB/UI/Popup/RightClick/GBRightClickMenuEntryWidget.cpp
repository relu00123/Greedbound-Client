// Fill out your copyright notice in the Description page of Project Settings.


#include "GB/UI/Popup/RightClick/GBRightClickMenuEntryWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Subsystem/UI/GBPopupManagerSubsystem.h"

void UGBRightClickMenuEntryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (EntryButton)
    {
        EntryButton->OnClicked.AddDynamic(this, &UGBRightClickMenuEntryWidget::HandleClick);
    }
}

void UGBRightClickMenuEntryWidget::SetEntry(const FRightClickMenuEntry& InEntry)
{
    Entry = InEntry;

    if (LabelText)
    {
        LabelText->SetText(Entry.DisplayText);
    }
}
 
void UGBRightClickMenuEntryWidget::HandleClick()
{
    OnMenuEntryClicked.Broadcast(Entry.Action);
    UGBPopupManagerSubsystem* PopupManager = GetGameInstance()->GetSubsystem<UGBPopupManagerSubsystem>();
    PopupManager->CloseTransientPopup();
}
