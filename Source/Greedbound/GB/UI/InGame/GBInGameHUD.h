// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GBInGameHUD.generated.h"

class UAbilitySystemComponent;
class UGBPlayerHUDWidget;
class UTexture2D;
struct FGameplayTag;
struct FGameplayEventData;

/*
*  UI CommonUI로 대체 후 삭제 예정
*/
UCLASS()
class GREEDBOUND_API AGBInGameHUD : public AHUD
{
    GENERATED_BODY()

public:
    void                                SetAbilitySystemComponent(UAbilitySystemComponent* ASC);
    void                                CreatePlayerHUDWidget();
    void                                SetSkillIconTexture(const TArray<TPair<FGameplayTag, UTexture2D*>>& Iconlist);
    
private:
    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UGBPlayerHUDWidget>     PlayerHUDWidgetClass;
    
    UPROPERTY()
    TObjectPtr<UGBPlayerHUDWidget>      PlayerHUDWidget;
};
