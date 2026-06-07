// Fill out your copyright notice in the Description page of Project Settings.

#include "GBAsyncAction_PushSoftWidget.h"
#include "Define/GBDefine.h"
#include "Subsystem/UI/GBUISubsystem.h"
#include "UI/InGame/GBActivatableBaseWidget.h"

UGBAsyncAction_PushSoftWidget* UGBAsyncAction_PushSoftWidget::PushSoftWidget(const UObject* WorldContextObject, APlayerController* OwningPlayerController, TSoftClassPtr<UGBActivatableBaseWidget> InSoftWidgetClass, UPARAM(meta = (Categories = "UI.WidgetStack"))FGameplayTag InWidgetStackTag, bool bFocusOnNewlyPushedWidget /*= true*/)
{
    if (InSoftWidgetClass.IsNull())
    {
        GB_LOG(TEXT("InSoftWidgetClass is Invalid!"));
    }

    if (GEngine)
    {
        UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
        if (World)
        {
            UGBAsyncAction_PushSoftWidget* Node = NewObject<UGBAsyncAction_PushSoftWidget>();
            Node->CachedWorld = World;
            Node->CachedOwningPC = OwningPlayerController;
            Node->CachedSoftWidgetClass = InSoftWidgetClass;
            Node->CachedWidgetStackTag = InWidgetStackTag;
            Node->bCachedFocusOnNewlyPushedWidget = bFocusOnNewlyPushedWidget;

            Node->RegisterWithGameInstance(World);

            return Node;
        }
    }

    return nullptr;
}

void UGBAsyncAction_PushSoftWidget::Activate()
{
    UGBUISubsystem* UISubSystem = UGBUISubsystem::Get(CachedWorld.Get());
    GB_NULL_CHECK(UISubSystem);

    UISubSystem->PushSoftWidgetToStackAynsc(CachedWidgetStackTag, CachedSoftWidgetClass,
        [this](EGBAsyncPushWidgetState InPushState, UGBActivatableBaseWidget* PushedWidget)
        {
            switch (InPushState)
            {
            case EGBAsyncPushWidgetState::OnCreatedBeforePush:
                PushedWidget->SetOwningPlayer(CachedOwningPC.Get());
                OnWidgetCreatedBeforePush.Broadcast(PushedWidget);
                break;
            case EGBAsyncPushWidgetState::AfterPush:
                OnWidgetCreatedAfterPush.Broadcast(PushedWidget);

                SetReadyToDestroy();
                break;
            }
        }
    );
}
