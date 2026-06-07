// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GBUISubsystem.generated.h"

class UAbilitySystemComponent;
class UTexture2D;
class UGBPrimaryLayoutWidget;
class UGBActivatableBaseWidget;
class UGBCommonButtonBase;
struct FGameplayTag;
enum class EGBConfirmScreenType : uint8;
enum class EGBConfirmScreenButtonType : uint8;

enum class EGBAsyncPushWidgetState : uint8
{
    OnCreatedBeforePush,
    AfterPush
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnButtonDescriptionTextUpdatedDelegate, UGBCommonButtonBase*, BroadcastingButton, FText, DescriptionText);

UCLASS()
class GREEDBOUND_API UGBUISubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    static UGBUISubsystem*                  Get(const UObject* WorldContextObject);
    virtual bool                            ShouldCreateSubsystem(UObject* Outer) const override final;

    void                                    PushSoftWidgetToStackAynsc(const FGameplayTag& InWidgetStackTag, TSoftClassPtr<UGBActivatableBaseWidget> InSoftWidgetClass, TFunction<void(EGBAsyncPushWidgetState, UGBActivatableBaseWidget*)> AsyncPushStateCallback);
    void                                    PushConfirmScreenToModalStackAynsc(EGBConfirmScreenType InScreenType, const FText& InScreenTitle, const FText& InScreenMessage, TFunction<void(EGBConfirmScreenButtonType)> ButtonClikedCallback);
    
    UFUNCTION(BlueprintCallable)
    void                                    RegisterCreatedPrimaryLayoutWidget(UGBPrimaryLayoutWidget* InCreatedWidget);

    UFUNCTION(BlueprintCallable)
    void                                    SetHUDWidget(UAbilitySystemComponent* ASC);

    UFUNCTION(BlueprintCallable)
    void                                    SetHUDVisible(bool bIsVisible);

public:
    UPROPERTY(BlueprintAssignable)
    FOnButtonDescriptionTextUpdatedDelegate OnButtonDescriptionTextUpdated;

private:
    UPROPERTY(Transient)
    TObjectPtr<UGBPrimaryLayoutWidget>      CreatedPrimaryLayoutWidget;
};
