// Fill out your copyright notice in the Description page of Project Settings.

#include "GBUISubsystem.h"
#include "GameplayTagContainer.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Data/GBAssetManager.h"
#include "Define/GBDefine.h"
#include "Define/GBTags.h"
#include "Define/UI/GBCommonUITypes.h"
#include "UI/GBPrimaryLayoutWidget.h"
#include "UI/InGame/GBActivatableBaseWidget.h"
#include "UI/InGame/GBConfirmScreenWidget.h"
#include "UI/GBUIFunctionLibrary.h"

UGBUISubsystem* UGBUISubsystem::Get(const UObject* WorldContextObject)
{
    if (GEngine)
    {
        UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
        return UGameInstance::GetSubsystem<UGBUISubsystem>(World->GetGameInstance());
    }

    return nullptr;
}

bool UGBUISubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance() == false)
    {
        TArray<UClass*> FoundClasses;
        GetDerivedClasses(GetClass(), FoundClasses);

        return FoundClasses.IsEmpty();
    }

    return false;
}

void UGBUISubsystem::RegisterCreatedPrimaryLayoutWidget(UGBPrimaryLayoutWidget* InCreatedWidget)
{
    GB_NULL_CHECK(InCreatedWidget);
    CreatedPrimaryLayoutWidget = InCreatedWidget;
}

void UGBUISubsystem::PushSoftWidgetToStackAynsc(const FGameplayTag& InWidgetStackTag, TSoftClassPtr<UGBActivatableBaseWidget> InSoftWidgetClass, TFunction<void(EGBAsyncPushWidgetState, UGBActivatableBaseWidget*)> AsyncPushStateCallback)
{
    GB_CONDITION_CHECK(InSoftWidgetClass.IsNull() == false);

    UGBAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
        InSoftWidgetClass.ToSoftObjectPath(),
        FStreamableDelegate::CreateLambda(
            [this, InSoftWidgetClass, InWidgetStackTag, AsyncPushStateCallback]()
            {
                UClass* LoadedWidgetClass = InSoftWidgetClass.Get();
                GB_NULL_CHECK(LoadedWidgetClass);
                GB_NULL_CHECK(CreatedPrimaryLayoutWidget);

                UCommonActivatableWidgetContainerBase* FoundWidgetStack = CreatedPrimaryLayoutWidget->FindWidgetStackByTag(InWidgetStackTag);
                GB_NULL_CHECK(FoundWidgetStack);

                UCommonActivatableWidget* ActivatedWidget = FoundWidgetStack->GetActiveWidget();
                if (ActivatedWidget && ActivatedWidget->IsA(LoadedWidgetClass))
                {
                    ActivatedWidget->DeactivateWidget();
                    return;
                }

                UGBActivatableBaseWidget* CreatedWidget = FoundWidgetStack->AddWidget<UGBActivatableBaseWidget>(
                    LoadedWidgetClass,
                    [AsyncPushStateCallback](UGBActivatableBaseWidget& CreatedWidgetInstance)
                    {
                        AsyncPushStateCallback(EGBAsyncPushWidgetState::OnCreatedBeforePush, &CreatedWidgetInstance);
                    }
                );

                AsyncPushStateCallback(EGBAsyncPushWidgetState::AfterPush, CreatedWidget);
            }
        )
    );
}

void UGBUISubsystem::PushConfirmScreenToModalStackAynsc(EGBConfirmScreenType InScreenType, const FText& InScreenTitle, const FText& InScreenMessage, TFunction<void(EGBConfirmScreenButtonType)> ButtonClikedCallback)
{
    UGBConfirmScreenInfoObject* CreatedScreenInfoObject = nullptr;

    switch (InScreenType)
    {
    case EGBConfirmScreenType::Ok:
        CreatedScreenInfoObject = UGBConfirmScreenInfoObject::CreateOkScreenInfoObject(InScreenTitle, InScreenMessage);
        break;
    case EGBConfirmScreenType::YesNo:
        CreatedScreenInfoObject = UGBConfirmScreenInfoObject::CreateYesNoScreenInfoObject(InScreenTitle, InScreenMessage);
        break;
    case EGBConfirmScreenType::OkCancel:
        CreatedScreenInfoObject = UGBConfirmScreenInfoObject::CreateOkCancelScreenInfoObject(InScreenTitle, InScreenMessage);
        break;
    default:
        break;
    }

    GB_NULL_CHECK(CreatedScreenInfoObject);

    PushSoftWidgetToStackAynsc(GBTag::UI_WidgetStack_Modal, UGBUIFunctionLibrary::GetSoftWidgetClassByTag(GBTag::UI_Widget_ConfirmScreen),
        [CreatedScreenInfoObject, ButtonClikedCallback](EGBAsyncPushWidgetState InPushState, UGBActivatableBaseWidget* PushedWidget)
        {
            if (InPushState == EGBAsyncPushWidgetState::OnCreatedBeforePush)
            {
                UGBConfirmScreenWidget* CreatedConfirmScreenWidget = Cast<UGBConfirmScreenWidget>(PushedWidget);
                GB_NULL_CHECK(CreatedConfirmScreenWidget);

                CreatedConfirmScreenWidget->InitConfirmScreen(CreatedScreenInfoObject, ButtonClikedCallback);
            }
        }
    );
}

void UGBUISubsystem::SetHUDWidget(UAbilitySystemComponent* ASC)
{
    GB_NULL_CHECK(ASC);
    GB_NULL_CHECK(CreatedPrimaryLayoutWidget);

    CreatedPrimaryLayoutWidget->SetHUDWidget(ASC);
}

void UGBUISubsystem::SetHUDVisible(bool bIsVisible)
{
    if (CreatedPrimaryLayoutWidget)
    {
        CreatedPrimaryLayoutWidget->SetHUDVisible(bIsVisible);
    }
}
