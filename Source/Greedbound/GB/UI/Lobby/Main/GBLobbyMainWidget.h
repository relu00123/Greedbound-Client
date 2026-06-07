// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Define/Party/GBPartyType.h"
#include "Define/GamePhase/GBClientGamePhase.h"
#include "Define/Match/GBMatchQueueTypes.h"
#include "GBLobbyMatchStartButtonState.h"
#include "GBLobbyMainWidget.generated.h"

class UButton;
class UTextBlock;
class UGBLobbyMatchStartButton;

/**
 * 
 */
UCLASS()
class GREEDBOUND_API UGBLobbyMainWidget : public UUserWidget
{
    GENERATED_BODY()
    
public:
    void                                    OnGameStartClicked();

protected:

    virtual void                            NativeConstruct() override;

    virtual void                            NativeDestruct() override; 

private:
    void                                    HandleOnMatchQueueCanceled(const FClientQueueState& QueueState);

    void                                    HandleOnMatchQueueJoined(const FClientQueueState& QueueState);

    void                                    HandleOnMyPartyRoleChanged(EPartyRole NewPartyRole);

    void                                    HandleOnMyGamePhaseChanged(EGBClientGamePhase From, EGBClientGamePhase To);


protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UGBLobbyMatchStartButton>    MatchStartButton;


private:    
    TArray<TFunction<void()>>               MatchStartButtonHandlers;

    FDelegateHandle                         OnMyPartyRoleChangedHandle; 

    FDelegateHandle                         OnMatchQueueJoinedHandle;

    FDelegateHandle                         OnMatchQueueCanceledHandle;

    FDelegateHandle                         OnMyGamePhaseChangedHandle;

    FDelegateHandle                         OnGameStartClickedHandle;

  
    EGBLobbyMatchQueueState                 MatchState;

    void                                    MatchStartButtonClicked_At_PartyHostIdle();
    void                                    MatchStartButtonClicked_At_PartyHostSearching();
    void                                    MatchStartButtonClicked_At_PartyMemberIdle();
    void                                    MatchStartButtonClicked_At_PartyMemberSearching();
    void                                    MatchStartButtonClicked_At_SoloIdle();
    void                                    MatchStartButtonClicked_At_SoloSearching();
    void                                    MatchStartButtonClicked_At_MatchAssigned();
    void                                    MatchStartButtonClicked_At_Pending();
};
