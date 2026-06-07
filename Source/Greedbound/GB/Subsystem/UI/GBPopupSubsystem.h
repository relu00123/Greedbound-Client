// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GBPopupSubsystem.generated.h"

class UGBCommonPopupWidget;

UCLASS()
class GREEDBOUND_API UGBPopupSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:
//    UGBPopupSubsystem();
//    
//    virtual void                    Initialize(FSubsystemCollectionBase& Collection) override; // 필요없음 
//    void                            ShowPopup(const FString& Message, bool bUseCancel, TFunction<void()> ConfirmFn, TFunction<void()> CancelFn = nullptr);
//
//private:
//    UPROPERTY()
//    TSubclassOf<UGBCommonPopupWidget> PopupWidgetClass; // 해결완료  
//
//    UPROPERTY()
//    TObjectPtr<UGBCommonPopupWidget>    ActivePopupWidget; // 이변수는 쓰면 안될 것 같음! 
};
