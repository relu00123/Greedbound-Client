// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UI/Popup/Utils/GBPopupConfig.h"
#include "Define/Party/GBPartyType.h"
#include "GBPopupManagerSubsystem.generated.h"


class UGBOutsideClickCatcherWidget;
class UGBCommonPopupWidget;
class UGBSharedPopupCanvasWidget;
class UCanvasPanel;

UCLASS()
class GREEDBOUND_API UGBPopupManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    FORCEINLINE void                            SetOutsideClickCatcherClass(TSubclassOf<UGBOutsideClickCatcherWidget> InClass) { OutsideClickCatcherWidgetClass = InClass; }
    FORCEINLINE void                            SetSharedPopupCanvasWidgetClass(TSubclassOf<UGBSharedPopupCanvasWidget> InClass) { SharedPopupCanvasWidgetClass = InClass; }
    FORCEINLINE void                            SetCommonPopupWidgetClass(TSubclassOf<UGBCommonPopupWidget> InClass) { CommonPopupWidgetClass = InClass; }

    void                                        ShowTransientPopup(UUserWidget* PopupWidget, const FVector2D& ScreenPos);
    void                                        CloseTransientPopup();
    void                                        ShowPopup(const FGBPopupConfig& Config);

    void                                        HandleActivePartyInvite(const FPartyInvite& Invite);

protected:
    virtual void                                Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void                                Deinitialize() override;

private:
    void                                        TryPresentNextCommonPopup();
    void                                        PresentCommonPopup(const FGBQueuedPopup& Item);
    void                                        FinishActiveCommonPopup(bool bExecuteConfirm, bool bExecuteCancel, bool bExecuteTimeout);
    bool                                        DequeueCommonPopup(FGBQueuedPopup& Out);

private:
    UPROPERTY(EditAnywhere)
    TSubclassOf<UGBSharedPopupCanvasWidget>     SharedPopupCanvasWidgetClass;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UGBCommonPopupWidget>           CommonPopupWidgetClass;

    UPROPERTY()
    TObjectPtr<UGBSharedPopupCanvasWidget>      SharedPopupCanvas;

    UPROPERTY()
    TSubclassOf<UGBOutsideClickCatcherWidget>   OutsideClickCatcherWidgetClass;

    UPROPERTY()
    TObjectPtr<UUserWidget>                     TransientPopup;

    UPROPERTY()
    TObjectPtr<UGBOutsideClickCatcherWidget>   OutsideClickCatcher;

    FDelegateHandle                             ActiveInviteChangedHandle;

    // CommonPopup의 Pending Queue
    UPROPERTY()
    TArray<FGBQueuedPopup>                      PendingCommonPopup;

    // 현재 표시 중인 CommonPopup
    UPROPERTY()
    TObjectPtr<UGBCommonPopupWidget>            ActiveCommonPopupWidget = nullptr;

    // CommonPopup의 상태 관리
    bool                                        bCommonPopupActive = false;

    FTimerHandle                                TimeoutHandle;
};
