// Fill out your copyright notice in the Description page of Project Settings.

#include "GBPopupSubsystem.h"
#include "UI/Popup/Common/GBCommonPopupWidget.h"
#include "Define/GBDefine.h"
#include "UI/Popup/Utils/GBPopupConfig.h"

//UGBPopupSubsystem::UGBPopupSubsystem()
//    : PopupWidgetClass(nullptr)
//    , ActivePopupWidget(nullptr)
//{
//    // 이쪽 부분 하드코딩이긴 한데.. 음.. 일단 그냥 사용. 어처피 GameInstance에서 지정해주는 것도 하드코딩이 아닌가? 
//    static ConstructorHelpers::FClassFinder<UGBCommonPopupWidget> WidgetClassFinder(TEXT("/Game/GB/UI/Popup/Common/WBP_CommonPopup"));
//    if (WidgetClassFinder.Succeeded())
//    {
//        PopupWidgetClass = WidgetClassFinder.Class;
//    }
//}
//
//void UGBPopupSubsystem::Initialize(FSubsystemCollectionBase& Collection)
//{
//    Super::Initialize(Collection);
//    UE_LOG(LogTemp, Warning, TEXT("UGBPopupSubsystem initialized"));
//}
//
//void UGBPopupSubsystem::ShowPopup(const FString& Message, bool bUseCancel, TFunction<void()> ConfirmFn, TFunction<void()> CancelFn)
//{
//    UE_LOG(LogTemp, Warning, TEXT("ShowPopup Function Called!"));
//
//    if (!PopupWidgetClass)
//    {
//        UE_LOG(LogTemp, Warning, TEXT("PopupWidget Class Not found"));
//    }
//
//    if (ActivePopupWidget)
//    {
//        ActivePopupWidget->RemoveFromParent();
//        ActivePopupWidget = nullptr;
//    }
//
//    UWorld* World = GetWorld();
//    GB_VALID_CHECK_WITHOUT_LOG(World);
//
//    ActivePopupWidget = CreateWidget<UGBCommonPopupWidget>(World, PopupWidgetClass);
//    GB_NULL_CHECK_WITHOUT_LOG(ActivePopupWidget);
//
//    FGBPopupConfig popupConfig;
//    popupConfig.Message = Message;
//    popupConfig.bUseCancel = bUseCancel;
//    popupConfig.OnConfirm = ConfirmFn;
//    popupConfig.OnCancel = CancelFn;
//
//    ActivePopupWidget->SetupPopup(popupConfig);
//
//    APlayerController* PC = GetWorld()->GetFirstPlayerController();
//    if (PC)
//    {
//        FInputModeGameAndUI InputMode;
//        InputMode.SetWidgetToFocus(ActivePopupWidget->TakeWidget());
//        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
//        PC->SetInputMode(InputMode);
//        PC->bShowMouseCursor = true;
//    }
//
//    ActivePopupWidget->AddToViewport(9999);
//}
