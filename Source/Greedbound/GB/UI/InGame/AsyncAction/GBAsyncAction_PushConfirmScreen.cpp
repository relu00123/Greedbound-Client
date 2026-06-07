// Fill out your copyright notice in the Description page of Project Settings.

#include "GBAsyncAction_PushConfirmScreen.h"
#include "Define/GBDefine.h"
#include "Subsystem/UI/GBUISubsystem.h"

UGBAsyncAction_PushConfirmScreen* UGBAsyncAction_PushConfirmScreen::PushConfirmScreen(const UObject* WorldContextObject, EGBConfirmScreenType ScreenType, FText InScreenTitle, FText InScreenMessage)
{
    if (GEngine)
    {
        UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
        GB_NULL_CHECK_WITH_RETURN(World, nullptr);

        UGBAsyncAction_PushConfirmScreen* AsyncAction = NewObject<UGBAsyncAction_PushConfirmScreen>();
        AsyncAction->CachedWorld = World;
        AsyncAction->CachedScreenType = ScreenType;
        AsyncAction->CachedScreenTitle = InScreenTitle;
        AsyncAction->CachedScreenMessage = InScreenMessage;

        AsyncAction->RegisterWithGameInstance(World);

        return AsyncAction;
    }

    return nullptr;
}

void UGBAsyncAction_PushConfirmScreen::Activate()
{
    UGBUISubsystem::Get(CachedWorld.Get())->PushConfirmScreenToModalStackAynsc(
        CachedScreenType, 
        CachedScreenTitle, 
        CachedScreenMessage,
        [this](EGBConfirmScreenButtonType ClickedButtonType)
        {
            OnButtonClicked.Broadcast(ClickedButtonType);

            SetReadyToDestroy();
        }
    );
}
