// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GBRightClickMenuTypes.h"
#include "GBRightClickMenuWidget.generated.h"

class UVerticalBox;
class UGBRightClickMenuEntryWidget;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMenuActionSelected, ERightClickMenuAction, Action);

UCLASS()
class GREEDBOUND_API UGBRightClickMenuWidget : public UUserWidget
{
    GENERATED_BODY()
    
public :
    // 메뉴 항목을 체우는 함수
    void                                        SetupEntries(const TArray<FRightClickMenuEntry>& Entries, const FString& TargetNickName); 

    UPROPERTY(BlueprintAssignable)
    FOnMenuActionSelected                       OnMenuActionSelected;

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UVerticalBox>                    MenuContainer;


    UPROPERTY(EditAnywhere, Category = "Entry") 
    TSubclassOf<UGBRightClickMenuEntryWidget>   EntryWidgetClass;

    UFUNCTION()
    void                                        HandleMenuEntryClicked(ERightClickMenuAction Action);
};
