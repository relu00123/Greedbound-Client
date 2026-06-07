// Fill out your copyright notice in the Description page of Project Settings.

#include "GBKeyRemapScreenWidget.h"
#include "Framework/Application/IInputProcessor.h"
#include "CommonRichTextBlock.h"
#include "CommonInputSubsystem.h"
#include "Define/GBDefine.h"

/*
* 재설정할 Key Input을 받아서 넘겨주는 클래스
*/
class FGBKeyRemapScreenInputPreprocessor : public IInputProcessor
{
public:
    DECLARE_DELEGATE_OneParam(FOnInputPreProcessorKeyPressedDelegate, const FKey& /*PressedKey*/)
    DECLARE_DELEGATE_OneParam(FOnInputPreProcessorKeySelectedCanceldDelegate, const FString& /*CanceledReason*/)

    FGBKeyRemapScreenInputPreprocessor(ECommonInputType InInputTypeToListenTo, ULocalPlayer* OwningLocalPlayer)
        : CachedInputTypeToListenTo(InInputTypeToListenTo)
        , CachedWeakOwningLocalPlayer(OwningLocalPlayer)
    {
    }

    void ProcessPressedKey(const FKey& InPressedKey)
    {
        if (InPressedKey == EKeys::Escape)
        {
            OnInputPreProcessorKeySelectedCanceld.ExecuteIfBound(TEXT("Key Remap has been canceled"));

            return;
        }

        UCommonInputSubsystem* CommonInputSubSystem = UCommonInputSubsystem::Get(CachedWeakOwningLocalPlayer.Get());
        GB_NULL_CHECK(CommonInputSubSystem);

        ECommonInputType CurrentInputType = CommonInputSubSystem->GetCurrentInputType();

        switch (CachedInputTypeToListenTo)
        {
        case ECommonInputType::MouseAndKeyboard:
        {
            if (InPressedKey.IsGamepadKey() || CurrentInputType == ECommonInputType::Gamepad)
            {
                OnInputPreProcessorKeySelectedCanceld.ExecuteIfBound(TEXT("Detected Gamepad Key pressed for keyboard inputs. Key Remap has been canceled"));

                return;
            }

            break;
        }
        case ECommonInputType::Gamepad:
        {
            // TODO : 키보드의 경우 경고 메시지를 띄울 조건 추가
            if (InPressedKey.IsGamepadKey() == false)
            {                OnInputPreProcessorKeySelectedCanceld.ExecuteIfBound(TEXT("Detected non Gamepad Key pressed for Gamepad inputs. Key Remap has been canceled"));

                return;
            }
            break;
        }
        default:
            break;
        }

        OnInputPreProcessorKeyPressed.ExecuteIfBound(InPressedKey);
    }

protected:
    virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override final
    {

    }

    virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override final
    {
        ProcessPressedKey(InKeyEvent.GetKey());

        return true;
    }

    virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override final
    {
        ProcessPressedKey(MouseEvent.GetEffectingButton());

        return true;
    }

public:
    FOnInputPreProcessorKeyPressedDelegate          OnInputPreProcessorKeyPressed;
    FOnInputPreProcessorKeySelectedCanceldDelegate  OnInputPreProcessorKeySelectedCanceld;

private:
    ECommonInputType                                CachedInputTypeToListenTo;

    TWeakObjectPtr<ULocalPlayer>                    CachedWeakOwningLocalPlayer;
};

void UGBKeyRemapScreenWidget::SetDesiredInputTypeToFilter(ECommonInputType InDesiredInputType)
{
    CachedDesiredInputType = InDesiredInputType;
}

void UGBKeyRemapScreenWidget::NativeOnActivated()
{
    Super::NativeOnActivated();

    CachedInputPreprecessor = MakeShared<FGBKeyRemapScreenInputPreprocessor>(CachedDesiredInputType, GetOwningLocalPlayer());
    CachedInputPreprecessor->OnInputPreProcessorKeyPressed.BindUObject(this, &UGBKeyRemapScreenWidget::OnValidKeyPressedDetected);
    CachedInputPreprecessor->OnInputPreProcessorKeySelectedCanceld.BindUObject(this, &UGBKeyRemapScreenWidget::OnKeySelectedCanceled);

    FSlateApplication::Get().RegisterInputPreProcessor(CachedInputPreprecessor, -1);

    FString InputDeviceName;

    switch (CachedDesiredInputType)
    {
    case ECommonInputType::MouseAndKeyboard:
        InputDeviceName = TEXT("Mouse & keyboard");
        break;
    case ECommonInputType::Gamepad:
        InputDeviceName = TEXT("Gamepad");
        break;
    default:
        break;
    }

    const FString DisplayRichMessage = FString::Printf(
        TEXT("<KeyRemapDefault>Press Any</> <KeyRemapHighlight>%s</> <KeyRemapDefault>key.</>"), *InputDeviceName
    );

    CRT_RemapMessage->SetText(FText::FromString(DisplayRichMessage));
}

void UGBKeyRemapScreenWidget::NativeOnDeactivated()
{
    Super::NativeOnDeactivated();

    if (CachedInputPreprecessor)
    {
        FSlateApplication::Get().UnregisterInputPreProcessor(CachedInputPreprecessor);

        CachedInputPreprecessor.Reset();
    }
}

void UGBKeyRemapScreenWidget::OnValidKeyPressedDetected(const FKey& PressedKey)
{
    RequestDeactivateWidget(
        [this, PressedKey]() 
        {
            OnKeyRemapScreenKeyPressed.ExecuteIfBound(PressedKey);
        }
    );
}

void UGBKeyRemapScreenWidget::OnKeySelectedCanceled(const FString& CanceledReason)
{
    RequestDeactivateWidget(
        [this, CanceledReason]()
        {
            OnKeyRemapScreenKeySelectedCanceled.ExecuteIfBound(CanceledReason);
        }
    );
}

void UGBKeyRemapScreenWidget::RequestDeactivateWidget(TFunction<void()> PreDeactivateCallback)
{
    FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateLambda(
            [this, PreDeactivateCallback](float DeltaTime)->bool
            {
                PreDeactivateCallback();

                DeactivateWidget();

                return false;
            }
        )
    );
}
