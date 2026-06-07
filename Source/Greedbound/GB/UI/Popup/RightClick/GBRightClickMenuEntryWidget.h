// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GBRightClickMenuTypes.h"
#include "GBRightClickMenuEntryWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRightClickMenuSelected, ERightClickMenuAction, Action);

UCLASS()
class GREEDBOUND_API UGBRightClickMenuEntryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void                        SetEntry(const FRightClickMenuEntry& InEntry);

    UPROPERTY(BlueprintAssignable)
    FOnRightClickMenuSelected   OnMenuEntryClicked;

protected:
    virtual                     void NativeConstruct() override;

    UFUNCTION()
    void                        HandleClick();

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton>         EntryButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock>      LabelText;

    FRightClickMenuEntry        Entry;
};
