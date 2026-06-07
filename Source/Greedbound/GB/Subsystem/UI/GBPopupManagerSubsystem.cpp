// Fill out your copyright notice in the Description page of Project Settings.


#include "GB/Subsystem/UI/GBPopupManagerSubsystem.h"
#include "UI/Popup/Utils/GBOutsideClickCatcherWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "UI/Popup/Utils/GBSharedPopupCanvasWidget.h"
#include "UI/Popup/Common/GBCommonPopupWidget.h"
#include "Subsystem/Social/GBPartyManager.h"

void UGBPopupManagerSubsystem::ShowTransientPopup(UUserWidget* PopupWidget, const FVector2D& ScreenPos)
{
    if (!OutsideClickCatcherWidgetClass || !PopupWidget)
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    // 기존에 열린 Transient popup이 있다면 닫기
    CloseTransientPopup(); 

    // OutsideClickCatcher 생성
    UGBOutsideClickCatcherWidget* ClickCatcher = CreateWidget<UGBOutsideClickCatcherWidget>(World, OutsideClickCatcherWidgetClass);
    if (!ClickCatcher)
    {
        return;
    }

    // 클릭시 CloseTransientPopup 실행
    ClickCatcher->BindOnClicked(FSimpleDelegate::CreateUObject(this, &UGBPopupManagerSubsystem::CloseTransientPopup));
    ClickCatcher->AddToViewport(50); 
    OutsideClickCatcher = ClickCatcher;

    // PopupWidget을 ClickCatcher의 Canvas에 직접 추가 
    if (UCanvasPanel* RootCanvas = ClickCatcher->GetRootCanvas())
    {
        if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(RootCanvas->AddChild(PopupWidget)))
        {
            CanvasSlot->SetPosition(ScreenPos);
            //CanvasSlot->SetAutoSize(true); // 필요시
        }
    }

    TransientPopup = PopupWidget;
}

void UGBPopupManagerSubsystem::CloseTransientPopup()
{
    if (TransientPopup)
    {
        TransientPopup->RemoveFromParent();
        TransientPopup = nullptr;
    }

    if (OutsideClickCatcher)
    {
        OutsideClickCatcher->RemoveFromParent();
        OutsideClickCatcher = nullptr;
    }
}

void UGBPopupManagerSubsystem::ShowPopup(const FGBPopupConfig& Config)
{
    // 리팩토링중.. (09.01) 기존로직 시작 
    //if (!CommonPopupWidgetClass)
    //{
    //    UE_LOG(LogTemp, Error, TEXT("PopupWidgetClass is not set!"));
    //    return;
    //}

    //UWorld* World = GetWorld();
    //if (!World) return;

    //UGBCommonPopupWidget* NewPopup = CreateWidget<UGBCommonPopupWidget>(World, CommonPopupWidgetClass);
    //if (!NewPopup)
    //{
    //    UE_LOG(LogTemp, Error, TEXT("Failed to create Popup Widget"));
    //    return; 
    //}

    //NewPopup->SetupPopup(Config);

    //// 나중에 주석 칠 라고 고민중.. 
    //APlayerController* PC = World->GetFirstPlayerController();
    //if (PC)
    //{
    //    FInputModeGameAndUI InputMode;
    //    InputMode.SetWidgetToFocus(NewPopup->TakeWidget());
    //    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

    //    PC->SetInputMode(InputMode);
    //    PC->bShowMouseCursor = true;
    //}

    //NewPopup->AddToViewport(9999); // 리팩토링 필요

    // 리팩토링중.. (09.01) 기존 로직 끝 

    FGBQueuedPopup Item;
    Item.Config = Config;
    PendingCommonPopup.Add(Item);
    TryPresentNextCommonPopup();
}

void UGBPopupManagerSubsystem::HandleActivePartyInvite(const FPartyInvite& Invite)
{
    // 파티초대 Popup을 띄우자. 

    FGBPopupConfig config;
 
    config.Message = FString::Printf(TEXT("%s invited you to the party"), *(Invite.InviterName));
    config.bUseCancel = true;
    config.bUseTimeout = true; 
    config.TimeoutSeconds = 10.0f;

    UGBPartyManager* PartyMgr = GetGameInstance()->GetSubsystem<UGBPartyManager>();
     

    config.OnConfirm = [WeakGI = TWeakObjectPtr<UGameInstance>(GetGameInstance())]()
        {
            if (!WeakGI.IsValid())
            {
                return;
            }
            if (UGBPartyManager* PM = WeakGI->GetSubsystem<UGBPartyManager>())
            {
                PM->AcceptCurrentInvite();
            }
        };
    config.OnCancel = [WeakGI = TWeakObjectPtr<UGameInstance>(GetGameInstance())]()
        {
            if (!WeakGI.IsValid())
            {
                return;
            }
            if (UGBPartyManager* PM = WeakGI->GetSubsystem<UGBPartyManager>())
            {
                PM->DeclineCurrentInvite();
            }
        };
    config.OnTimeOut = [WeakGI = TWeakObjectPtr<UGameInstance>(GetGameInstance())]()
        {
            if (!WeakGI.IsValid())
            {
                return;
            }
            if (UGBPartyManager* PM = WeakGI->GetSubsystem<UGBPartyManager>())
            {
                PM->DeclineCurrentInvite();
            }
        };

    ShowPopup(config);
}

void UGBPopupManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (UGBPartyManager* PartyMgr = GetGameInstance()->GetSubsystem<UGBPartyManager>())
    {
        ActiveInviteChangedHandle =
            PartyMgr->BindOnActivePartyInviteChanged(
                FOnActivePartyInviteChanged::FDelegate::CreateUObject(
                    this, &UGBPopupManagerSubsystem::HandleActivePartyInvite));
    }

    // Popup Layer 생성
    if (!SharedPopupCanvas)
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    SharedPopupCanvas = CreateWidget<UGBSharedPopupCanvasWidget>(World, SharedPopupCanvasWidgetClass);

    if (SharedPopupCanvas)
    {
        SharedPopupCanvas->AddToViewport(100);
    }
}

void UGBPopupManagerSubsystem::Deinitialize()
{
    if (auto* PartyMgr = GetGameInstance()->GetSubsystem<UGBPartyManager>())
    {
        PartyMgr->RemoveOnActivePartyInviteChanged(ActiveInviteChangedHandle);
    }

    Super::Deinitialize();
}

void UGBPopupManagerSubsystem::TryPresentNextCommonPopup()
{
    // 이미 활성중인 CommonPopup이 있다면 대기
    if (bCommonPopupActive )
    {
        return;
    }

    FGBQueuedPopup Next;
    if (!DequeueCommonPopup(Next))
    {
        return;
    }

    PresentCommonPopup(Next);
}

void UGBPopupManagerSubsystem::PresentCommonPopup(const FGBQueuedPopup& Item)
{
    if (!CommonPopupWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("PopupWidgetClass is not set!"));
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    { 
        return;
    }

    UGBCommonPopupWidget* NewPopup = CreateWidget<UGBCommonPopupWidget>(World, CommonPopupWidgetClass);
    if (!NewPopup)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create Popup Widget"));
        return;
    }

    bCommonPopupActive= true;
    ActiveCommonPopupWidget = NewPopup;

    // 콘텐츠 설정 
    NewPopup->SetupPopup(Item.Config);

    // 입력모드 적용.. (선택)
    // ApplyInputModeForPopup(NewPopup);

    //// 나중에 주석 칠 라고 고민중.. 
    //APlayerController* PC = World->GetFirstPlayerController();
    //if (PC)
    //{
    //    FInputModeGameAndUI InputMode;
    //    InputMode.SetWidgetToFocus(NewPopup->TakeWidget());
    //    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

    //    PC->SetInputMode(InputMode);
    //    PC->bShowMouseCursor = true;
    //}

    // 뷰포트에 추가 (최상단 Z)
    NewPopup->AddToViewport(9999); // 리팩토링 필요

    TWeakObjectPtr<UGBPopupManagerSubsystem> SelfWeak(this);

    NewPopup->BindOnConfirmed(FSimpleDelegate::CreateLambda([SelfWeak]() {
        if (!SelfWeak.IsValid()) return;

        SelfWeak->FinishActiveCommonPopup(/*Confirm*/true, /*Cancel*/false, /*Timeout*/false);

    }));

    NewPopup->BindOnCanceled(FSimpleDelegate::CreateLambda([SelfWeak]()
        {
            if (!SelfWeak.IsValid()) return;
            SelfWeak->FinishActiveCommonPopup(/*Confirm*/false, /*Cancel*/true, /*Timeout*/false);
        }));

 
    if (Item.Config.bUseTimeout && Item.Config.TimeoutSeconds > 0.f)
    {
        FTimerDelegate D;
        D.BindLambda([SelfWeak]() {
            if (!SelfWeak.IsValid()) return;
            SelfWeak->FinishActiveCommonPopup(/*Confirm*/false, /*Cancel*/false, /*Timeout*/true);
            });

        World->GetTimerManager().SetTimer(TimeoutHandle, D, Item.Config.TimeoutSeconds, false);
    }

    // 리팩토링중.. (09.01) 기존 로직 끝 
}

void UGBPopupManagerSubsystem::FinishActiveCommonPopup(bool bExecuteConfirm, bool bExecuteCancel, bool bExecuteTimeout)
{
    // 타이머 정리
    if (UWorld* World = GetWorld())
    {
        if (TimeoutHandle.IsValid())
        {
            World->GetTimerManager().ClearTimer(TimeoutHandle);
        }
    }
    
    // 콜백 실행
    if (ActiveCommonPopupWidget)
    {
        const FGBPopupConfig& Config = ActiveCommonPopupWidget->GetConfigRef();

        if (bExecuteConfirm && Config.OnConfirm)
        {
            Config.OnConfirm();
        }

        if (bExecuteCancel && Config.OnCancel)
        {
            Config.OnCancel();
        }

        if (bExecuteTimeout && Config.OnTimeOut)
        {
            Config.OnTimeOut();
        }

        ActiveCommonPopupWidget->RemoveFromParent();
        ActiveCommonPopupWidget = nullptr;
    }

    // RestoreInputMode();

    bCommonPopupActive = false;

    TryPresentNextCommonPopup();
}

bool UGBPopupManagerSubsystem::DequeueCommonPopup(FGBQueuedPopup& Out)
{
    if (PendingCommonPopup.Num() == 0)
    {
        return false;
    }

    Out = PendingCommonPopup[0];

    PendingCommonPopup.RemoveAt(0); // front pop
    return true;
}

//void UGBPopupManagerSubsystem::ApplyInputModeForPopup(UGBCommonPopupWidget* Widget)
//{
//    if (!Widget) return;
//    if (UWorld* World = GetWorld())
//    {
//        if (APlayerController* PC = World->GetFirstPlayerController())
//        {
//            // 이전 상태 백업 (단순화해도 됨)
//            if (!PrevShowMouseCursor.IsSet())
//                PrevShowMouseCursor = PC->bShowMouseCursor;
//
//            FInputModeGameAndUI InputMode;
//            InputMode.SetWidgetToFocus(Widget->TakeWidget());
//            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
//            PC->SetInputMode(InputMode);
//
//            PC->bShowMouseCursor = true;
//        }
//    }
//}
//
//void UGBPopupManagerSubsystem::RestoreInputMode()
//{
//    if (UWorld* World = GetWorld())
//    {
//        if (APlayerController* PC = World->GetFirstPlayerController())
//        {
//            // 프로젝트 정책에 맞게 단순화 가능
//            PC->SetInputMode(FInputModeGameOnly());
//            if (PrevShowMouseCursor.IsSet())
//            {
//                PC->bShowMouseCursor = PrevShowMouseCursor.GetValue();
//            }
//            else
//            {
//                PC->bShowMouseCursor = false;
//            }
//        }
//    }
//
//    PrevShowMouseCursor.Reset();
//}
//
//void UGBPopupManagerSubsystem::ClearAllPopups()
//{
//    // 타이머 제거
//    if (UWorld* World = GetWorld())
//    {
//        if (TimeoutHandle.IsValid())
//            World->GetTimerManager().ClearTimer(TimeoutHandle);
//    }
//
//    Pending.Reset();
//
//    if (ActivePopupWidget)
//    {
//        ActivePopupWidget->RemoveFromParent();
//        ActivePopupWidget = nullptr;
//    }
//
//    RestoreInputMode();
//    bPopupActive = false;
//}

 
