// Fill out your copyright notice in the Description page of Project Settings.


#include "GB/UI/Lobby/PlayerList/GBLobbyPlayerEntryWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "UI/Lobby/Data/GBPlayerEntryClassVisualData.h"
#include "UI/Popup/RightClick/GBRightClickMenuEntryWidget.h"
#include "UI/Popup/RightClick/GBRightClickMenuWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "UI/Lobby/GBLobbyHUD.h"
#include "Subsystem/UI/GBPopupManagerSubsystem.h"
#include "Subsystem/Social/GBPartyManager.h"
#include "UI/Popup/RightClick/GBRightClickMenuBuilder.h"
#include "Subsystem/Social/GBFriendManager.h"

void UGBLobbyPlayerEntryWidget::SetupUserInfo(const FUserInfo Info)
{
    if (!ClassVisualDataAsset)
    {
        UE_LOG(LogTemp, Warning, TEXT("ClassVisualDataAsset이 유효하지 않습니다."));
        return;
    }

    CurrentUserInfo = Info;

    bIsEmpty = false;
    bIsEmptyPartySlot = false;

    CharacterNameText->SetVisibility(ESlateVisibility::Visible);
    ClassNameText->SetVisibility(ESlateVisibility::Visible);
    ClassImage->SetVisibility(ESlateVisibility::Visible);
    ClassIconImage->SetVisibility(ESlateVisibility::Visible);
    StatusText->SetVisibility(ESlateVisibility::Visible);

    // 파티슬롯은 보이지 않도록 
    EmptyPartySlot->SetVisibility(ESlateVisibility::Collapsed);
    //PartyLeaderImage->SetVisibility(ESlateVisibility::Collapsed);


    CharacterNameText->SetText(FText::FromString(Info.NickName));

    FGBClassVisualData* targetData = ClassVisualDataAsset->ClassVisuals.Find(Info.ClassType);
    if (targetData)
    {
        ClassNameText->SetText(targetData->DisplayName);
        ClassImage->SetBrushFromTexture(targetData->Portrait);
        ClassIconImage->SetBrushFromTexture(targetData->Icon);
    }

    if (Info.bOnline)
    {
        const UEnum* GamePhaseEnum = StaticEnum<EGBClientGamePhase>();
        FString PhaseStr = TEXT("Online");

        if (GamePhaseEnum)
        {
            PhaseStr = GamePhaseEnum->GetNameStringByValue(static_cast<int64>(Info.ClientGamePhase));
        }

        StatusText->SetText(FText::FromString(PhaseStr));
    }

    else
    {
        StatusText->SetText(FText::FromString(TEXT("Offline")));
    }

 
}

void UGBLobbyPlayerEntryWidget::SetupUserClass(EGBCharacterClassType ClassType)
{
    CurrentUserInfo.ClassType = ClassType;

    FGBClassVisualData* targetData = ClassVisualDataAsset->ClassVisuals.Find(ClassType);
    if (targetData)
    {
        ClassNameText->SetText(targetData->DisplayName);
        ClassImage->SetBrushFromTexture(targetData->Portrait);
        ClassIconImage->SetBrushFromTexture(targetData->Icon);
    }
}

void UGBLobbyPlayerEntryWidget::SetupUserGamePhase(EGBClientGamePhase GamePhase)
{
    CurrentUserInfo.ClientGamePhase = GamePhase;

    FString PhaseStr = TEXT("None");

    if (const UEnum* GamePhaseEnum = StaticEnum<EGBClientGamePhase>())
    {
        PhaseStr = GamePhaseEnum->GetNameStringByValue(static_cast<int64>(GamePhase));
    }

    StatusText->SetText(FText::FromString(PhaseStr));

}

void UGBLobbyPlayerEntryWidget::SetPartyLeaderIcon(bool isLeader)
{
    if (isLeader)
    {
        PartyLeaderImage->SetVisibility(ESlateVisibility::Visible);
        bIsPartyHost = true;
    }
    else
    {
        PartyLeaderImage->SetVisibility(ESlateVisibility::Collapsed);
        bIsPartyHost = false;
    }
}
 

void UGBLobbyPlayerEntryWidget::SetEmptyPartySlot()
{
    CharacterNameText->SetVisibility(ESlateVisibility::Collapsed);
    ClassNameText->SetVisibility(ESlateVisibility::Collapsed);
    ClassImage->SetVisibility(ESlateVisibility::Collapsed);
    ClassIconImage->SetVisibility(ESlateVisibility::Collapsed);
    StatusText->SetVisibility(ESlateVisibility::Collapsed);
    PartyLeaderImage->SetVisibility(ESlateVisibility::Collapsed);

    // 파티슬롯만 보이도록
    EmptyPartySlot->SetVisibility(ESlateVisibility::Visible);
    bIsEmptyPartySlot = true; 
    bIsEmpty = false; 

    CurrentUserInfo = FUserInfo();
}

void UGBLobbyPlayerEntryWidget::SetEmptySlot()
{
    bIsEmpty = true;
    bIsEmptyPartySlot = false; 
    CharacterNameText->SetVisibility(ESlateVisibility::Collapsed);
    ClassNameText->SetVisibility(ESlateVisibility::Collapsed);
    ClassImage->SetVisibility(ESlateVisibility::Collapsed);
    ClassIconImage->SetVisibility(ESlateVisibility::Collapsed);
    StatusText->SetVisibility(ESlateVisibility::Collapsed);
    EmptyPartySlot->SetVisibility(ESlateVisibility::Collapsed);
    PartyLeaderImage->SetVisibility(ESlateVisibility::Collapsed);


    CurrentUserInfo = FUserInfo();
}

//void UGBLobbyPlayerEntryWidget::ChangeOnlineState(bool isOnline)
//{
//    if (!IsEmptySlot())
//    {
//        if (isOnline)
//            StatusText->SetText(FText::FromString(TEXT("Online")));
//        else
//            StatusText->SetText(FText::FromString(TEXT("Offline")));
//    }
//}

FReply UGBLobbyPlayerEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
    {
        if (!RightClickMenuClass) return FReply::Unhandled();

        UWorld* World = GetWorld();
        if (!World) return FReply::Unhandled();

        // 마우스 스크린 좌표 얻기
        FVector2D ScreenPos = InMouseEvent.GetScreenSpacePosition();

        // 메뉴 위젯 생성
        UGBRightClickMenuWidget* MenuWidget = CreateWidget<UGBRightClickMenuWidget>(World, RightClickMenuClass);
        if (!MenuWidget) return FReply::Unhandled();

        MenuWidget->OnMenuActionSelected.AddDynamic(this, &UGBLobbyPlayerEntryWidget::HandleMenuAction);

        // 메뉴 위젯 항목 구성
        TArray<FRightClickMenuEntry> MenuEntries = GBRightClickMenuBuilder::BuildForUser(CurrentUserInfo, GetGameInstance(), LobbyPlayerListType, IsEmpty());

        if (MenuEntries.Num() != 0)
        {
            MenuWidget->SetupEntries(MenuEntries, CurrentUserInfo.NickName);

            // PopupManagerSubsystem을 통해 등록
            if (UGBPopupManagerSubsystem* PopupManager = GetGameInstance()->GetSubsystem<UGBPopupManagerSubsystem>())
            {
                FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
                PopupManager->ShowTransientPopup(MenuWidget, MousePos);
            }
        }
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UGBLobbyPlayerEntryWidget::HandleMenuAction(ERightClickMenuAction Action)
{
    switch (Action)
    {
    case ERightClickMenuAction::AddToFriend:
        UE_LOG(LogTemp, Warning, TEXT("Add to Friend Selected!"));
        GetWorld()->GetGameInstance()->GetSubsystem<UGBFriendManager>()->SendAddFriendRequest(CurrentUserInfo.NickName);
        break;
    case ERightClickMenuAction::BlockUser:
        // Block logic
        break;
    case ERightClickMenuAction::AcceptFriendRequest:
        GetWorld()->GetGameInstance()->GetSubsystem<UGBFriendManager>()->SendRespondToFriendRequest(CurrentUserInfo.NickName, true);
        break;
    case ERightClickMenuAction::DeclineFriendRequest:
        GetWorld()->GetGameInstance()->GetSubsystem<UGBFriendManager>()->SendRespondToFriendRequest(CurrentUserInfo.NickName, false);
        break;
    case ERightClickMenuAction::RemoveFriend:
        GetWorld()->GetGameInstance()->GetSubsystem<UGBFriendManager>()->SendRemoveFriendRequest(CurrentUserInfo.NickName);
        break;
    case ERightClickMenuAction::InviteToParty:
        GetWorld()->GetGameInstance()->GetSubsystem<UGBPartyManager>()->SendInviteToPartyRequest(CurrentUserInfo.NickName);
        break;
    case ERightClickMenuAction::DelegatePartyHost:
        GetWorld()->GetGameInstance()->GetSubsystem<UGBPartyManager>()->SendPartyHostTransferRequest(CurrentUserInfo.NickName);
        break;
    case ERightClickMenuAction::KickFromParty:
        GetWorld()->GetGameInstance()->GetSubsystem<UGBPartyManager>()->SendKickFromPartyRequest(CurrentUserInfo.NickName); 
        break;
    case ERightClickMenuAction::LeaveFromParty:
        GetWorld()->GetGameInstance()->GetSubsystem<UGBPartyManager>()->SendLeaveFromPartyRequest(CurrentUserInfo.NickName);
    default:
        break;
    }
    // PopupManager에게 PopupUI를 닫아달라고 요청함. 
}
