// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "GBPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
class UTexture2D;
class UGBInputData;
class UGBAbilityTriggerInputData;
class UGBAbilitySystemComponent;
class UAbilitySystemComponent;
class UGBInGameWaitingWidget;
struct FInputActionValue;
struct FGameplayEventData;

UCLASS()
class GREEDBOUND_API AGBPlayerController : public APlayerController
{
    GENERATED_BODY()
    
public:
    UFUNCTION(BlueprintCallable)
    UGBAbilitySystemComponent*                  GetAbilitySystemComponent();

    UFUNCTION(Server, Reliable)
    void                                        Server_NotifyClientLoadedDungeonMap();
    void                                        HandleMatchStarted_Client();

protected:
    virtual void                                BeginPlay() override;
    virtual void                                OnPossess(APawn* aPawn) override final;
    virtual void                                OnRep_Pawn() override final;
    virtual void                                OnRep_PlayerState() override final;
    
    // UI
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void                                        BP_InitializePlayerUI();

private:
    virtual void                                SetupInputComponent() override;
    void                                        ShowDungeonWaitingHUD();
    void                                        TryNotifyReadyToPlay();

    void                                        AbilityTagTriggered(const FGameplayTag InputTag);
    void                                        AbilityTagReleased(const FGameplayTag InputTag);
    void                                        AbilityTagToggled(const FGameplayTag InputTag);
    void                                        OnBlockTagChanged(const FGameplayTag Tag, const int32 Count);

    // Bind InputActions
    void                                        Move(const FInputActionValue& InputActionValue);
    void                                        Look(const FInputActionValue& InputActionValue);

    void                                        TryLogin();
    void                                        HideWaitingHUD();
    void                                        ShowInGameHUD();

    UFUNCTION(Server, Reliable)
    void                                        ServerLogin(const FString& Token, const FString& CharacterId);

private:
    UPROPERTY(Transient)
    TObjectPtr<UGBAbilitySystemComponent>       AbilitySystemComponent;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UGBInputData>                    AbilityInputData;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    TArray<TObjectPtr<UInputMappingContext>>    DefaultMappingContexts;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    TObjectPtr<UInputAction>                    MoveAction;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    TObjectPtr<UInputAction>                    LookAction;

    UPROPERTY(EditDefaultsOnly, Category="UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UGBInGameWaitingWidget>         WaitingWidgetClass;

    UPROPERTY()
    TObjectPtr<UGBInGameWaitingWidget>          WaitingWidgetInstance;

    uint8                                       bMoveBlock : 1 = false;

    FTimerHandle                                TryNotifyReadyToPlayTickHandle;
};
