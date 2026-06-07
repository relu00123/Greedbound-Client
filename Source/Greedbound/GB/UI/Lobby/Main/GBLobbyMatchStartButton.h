// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GBLobbyMatchStartButtonState.h"
#include "GBLobbyMatchStartButton.generated.h"

class UButton;
class UTextBlock;
class UGBLobbyMatchStartButtonData;

UCLASS()
class GREEDBOUND_API UGBLobbyMatchStartButton : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable) 
    void                                        ApplyState(EGBLobbyMatchQueueState NewState);

    FORCEINLINE FDelegateHandle                 BindOnMatchStartClicked(const FOnMatchStartClicked::FDelegate& Delegate) { return OnMatchStartClicked.Add(Delegate); }
    FORCEINLINE void                            UnbindOnMatchStartClicked(FDelegateHandle Handle) { OnMatchStartClicked.Remove(Handle); }

protected:
    virtual void                                NativeConstruct() override;

private:
    UFUNCTION()
    void                                        HandleInternalClicked();

protected:
    UPROPERTY(meta= (BindWidget)) 
    TObjectPtr<UButton>                         StartButton;

    UPROPERTY(meta= (BindWidget))
    TObjectPtr<UTextBlock>                      Label;

    // 에디터에서 지정
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category ="Data")
    TObjectPtr<UGBLobbyMatchStartButtonData>    Data;

    FOnMatchStartClicked                        OnMatchStartClicked;
};
