#pragma once
#include "CoreMinimal.h"
#include "GBPopupConfig.generated.h"

USTRUCT(BlueprintType)
struct FGBPopupConfig
{
    GENERATED_BODY()
    
    // 본문 메세지 관련
    FString Message;

    // 수락 거절 버튼 관련 
    bool bUseCancel = false;
    TFunction<void()> OnConfirm;
    TFunction<void()> OnCancel;
    TFunction<void()> OnTimeOut;

    // 시간 제한 관련
    bool bUseTimeout = false;
    float TimeoutSeconds = 0.0f;

    // 생성자 (필요시)
    /*FPopupConfig() = default;

    FPopupConfig(const FString& InMessage, bool bInUseCancel = false)
        : Message(InMessage), bUseCancel(bInUseCancel)
    {
    }*/
};

USTRUCT(BlueprintType)
struct FGBQueuedPopup
{
    GENERATED_BODY()

    UPROPERTY()
    FGBPopupConfig Config;
};
