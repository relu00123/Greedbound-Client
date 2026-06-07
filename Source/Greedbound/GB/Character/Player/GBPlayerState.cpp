// Fill out your copyright notice in the Description page of Project Settings.

#include "GBPlayerState.h"
#include "Components/GBAbilitySystemComponent.h"
#include "Components/Player/GBInventoryComponent.h"
#include "Net/UnrealNetwork.h"

AGBPlayerState::AGBPlayerState(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    AbilitySystemComponent = CreateDefaultSubobject<UGBAbilitySystemComponent>("AbilitySystem");
    AbilitySystemComponent->SetIsReplicated(true);

    // Full    : 모든 GampleayEffect를 서버 -> 클라로 전파 
    // Minimal : Attributes만 전파하고, GameplayEffect 자체는 전파하지 않음
    // Mixed   : Attributes는 전파, GameplayEffect는 "중요한" 것만 전파 
    //
    // "중요한"의 정의 
    // 1. GameplayEffect가 bReplicateEffect 설정됨 ( FGameplayEffectSpec::bReplicateEffect = true) 
    // 2. GameplayEffect에 GameplayCues가 포함되어 있음
    // 3. GameplayEffectSpec이 Durational 
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    InventoryComponent = CreateDefaultSubobject<UGBInventoryComponent>("Inventory");
    InventoryComponent->SetIsReplicated(true);
    InventoryComponent->InitInventory(30);

    SetNetUpdateFrequency(100.f);
}

void AGBPlayerState::SetSelectedClass(EGBCharacterClassType InClass)
{
    // 서버에서만 설정할 수 있도록 한다.
    if (!HasAuthority())
    {
        return;
    }

    if (SelectedClass == InClass)
    {
        return;
    }

    SelectedClass = InClass;
}
 
void AGBPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AGBPlayerState, SelectedClass);
}
