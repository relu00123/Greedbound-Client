// Fill out your copyright notice in the Description page of Project Settings.

#include "GBInGameHUD.h"
#include "Define/GBTags.h"
#include "Define/GBDefine.h"
#include "UI/InGame/PlayerInfo/GBPlayerHUDWidget.h"
#include "AbilitySystem/Attribute/GBHealthAttributeSet.h"
#include "AbilitySystem/Attribute/GBSpeedAttributeSet.h"

void AGBInGameHUD::SetAbilitySystemComponent(UAbilitySystemComponent* ASC)
{
    GB_NULL_CHECK(ASC);

    AbilitySystemComponent = ASC;
}

void AGBInGameHUD::CreatePlayerHUDWidget()
{
    if (PlayerHUDWidgetClass)
    {
        PlayerHUDWidget = CreateWidget<UGBPlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass);
        if (PlayerHUDWidget)
        {
            if (AbilitySystemComponent)
            {
                PlayerHUDWidget->InitializeAttributeInfo(AbilitySystemComponent);

                AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UGBHealthAttributeSet::GetCurrentHealthAttribute())
                    .AddUObject(PlayerHUDWidget, &UGBPlayerHUDWidget::OnHealthAttributeChanged);
                AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UGBHealthAttributeSet::GetMaxHealthAttribute())
                    .AddUObject(PlayerHUDWidget, &UGBPlayerHUDWidget::OnHealthAttributeChanged);
                AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UGBSpeedAttributeSet::GetCurrnetStaminaAttribute())
                    .AddUObject(PlayerHUDWidget, &UGBPlayerHUDWidget::OnSpeedAttributeChanged);
                AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UGBSpeedAttributeSet::GetMaxStaminaAttribute())
                    .AddUObject(PlayerHUDWidget, &UGBPlayerHUDWidget::OnSpeedAttributeChanged);

                const FGameplayTagContainer& BuffTags = UGameplayTagsManager::Get().RequestGameplayTagChildren(GBTag::State_Buff);
                const FGameplayTagContainer& DebuffTags = UGameplayTagsManager::Get().RequestGameplayTagChildren(GBTag::State_Debuff);
                const FGameplayTagContainer& CooldownTags = UGameplayTagsManager::Get().RequestGameplayTagChildren(GBTag::State_Cooldown);

                for(const FGameplayTag BuffTag : BuffTags)
                {
                    AbilitySystemComponent->RegisterGameplayTagEvent(BuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(PlayerHUDWidget, &UGBPlayerHUDWidget::OnBuffStateChanged);
                }

                for (const FGameplayTag DebuffTag : DebuffTags)
                {
                    AbilitySystemComponent->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(PlayerHUDWidget, &UGBPlayerHUDWidget::OnBuffStateChanged);
                }

                for (const FGameplayTag CooldownTag : CooldownTags)
                {
                    AbilitySystemComponent->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(PlayerHUDWidget, &UGBPlayerHUDWidget::OnSkillCooldownChanged);
                }

                AbilitySystemComponent->RegisterGameplayTagEvent(GBTag::State_Combat_Equipped, EGameplayTagEventType::NewOrRemoved).AddUObject(PlayerHUDWidget, &UGBPlayerHUDWidget::OnSkillInfoChanged);
            }

            PlayerHUDWidget->AddToViewport();
        }
    }
}

void AGBInGameHUD::SetSkillIconTexture(const TArray<TPair<FGameplayTag, UTexture2D*>>& Iconlist)
{
    PlayerHUDWidget->SetSkillIconTexture(Iconlist);
}
