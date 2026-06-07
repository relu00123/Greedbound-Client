// Fill out your copyright notice in the Description page of Project Settings.

#include "GBPlayerHUDWidget.h"
#include "Components/GBAbilitySystemComponent.h"
#include "Components/ProgressBar.h"
#include "Components/HorizontalBox.h"
#include "Define/GBDefine.h"
#include "Define/GBTags.h"
#include "AbilitySystem/Attribute/GBHealthAttributeSet.h"
#include "AbilitySystem/Attribute/GBSpeedAttributeSet.h"
#include "UI/InGame/PlayerInfo/GBDurationIconWidget.h"

void UGBPlayerHUDWidget::InitializeAttributeInfo(UAbilitySystemComponent* ASC)
{
    AbilitySystemComponent = Cast<UGBAbilitySystemComponent>(ASC);
    GB_NULL_CHECK(AbilitySystemComponent);

    const UGBHealthAttributeSet* HealthAttributeSet = ASC->GetSet<UGBHealthAttributeSet>();
    if (HealthAttributeSet)
    {
        CurrentHealth = HealthAttributeSet->GetCurrentHealth();
        MaxHealth = HealthAttributeSet->GetMaxHealth();
        UpdateProgressbar(Pb_HPBar, CurrentHealth / MaxHealth);
    }

    const UGBSpeedAttributeSet* SpeedAttributeSet = ASC->GetSet<UGBSpeedAttributeSet>();
    if (SpeedAttributeSet)
    {
        CurrentStamina = SpeedAttributeSet->GetCurrnetStamina();
        MaxStamina = SpeedAttributeSet->GetMaxStamina();
        UpdateProgressbar(Pb_StaminaBar, CurrentHealth / MaxHealth);
    }

    const TArray<UWidget*>& BuffChilds = HB_Buff->GetAllChildren();
    for (UWidget* ChildWidget : BuffChilds)
    {
        ChildWidget->SetRenderOpacity(0.f);
    }

    const TArray<UWidget*>& SkillChilds = HB_Skill->GetAllChildren();
    for (UWidget* ChildWidget : SkillChilds)
    {
        if (UGBDurationIconWidget* IconWidget = Cast<UGBDurationIconWidget>(ChildWidget))
        {
            IconWidget->SetRemainTextVisible(false);
        }
    }

    UpdateSkillIconImage();
}

void UGBPlayerHUDWidget::OnHealthAttributeChanged(const FOnAttributeChangeData& Payload)
{
    if (Payload.Attribute == UGBHealthAttributeSet::GetCurrentHealthAttribute())
    {
        CurrentHealth = Payload.NewValue;
        UpdateProgressbar(Pb_HPBar, CurrentHealth / MaxHealth);
    }
    else if (Payload.Attribute == UGBHealthAttributeSet::GetMaxHealthAttribute())
    {
        MaxHealth = Payload.NewValue;
        UpdateProgressbar(Pb_HPBar, CurrentHealth / MaxHealth);
    }
}

void UGBPlayerHUDWidget::OnSpeedAttributeChanged(const FOnAttributeChangeData& Payload)
{
    if (Payload.Attribute == UGBSpeedAttributeSet::GetCurrnetStaminaAttribute())
    {
        CurrentStamina = Payload.NewValue;
        UpdateProgressbar(Pb_StaminaBar, CurrentStamina / MaxStamina);
    }
    else if (Payload.Attribute == UGBSpeedAttributeSet::GetMaxStaminaAttribute())
    {
        MaxStamina = Payload.NewValue;
        UpdateProgressbar(Pb_StaminaBar, CurrentStamina / MaxStamina);
    }
}

void UGBPlayerHUDWidget::OnBuffStateChanged(const FGameplayTag Tag, const int32 Count)
{
    EGBBuffType BuffType = GetBuffTypeByTag(Tag);
    GB_CONDITION_CHECK(BuffType != EGBBuffType::None);

    const bool bIsAdded = Count > 0;
    if (bIsAdded)
    {
        CurrentBuffState.Add(BuffType);
    }
    else
    {
        if (CurrentBuffState.Contains(BuffType))
        {
            CurrentBuffState.Remove(BuffType);
        }
    }

    GB_NULL_CHECK(HB_Buff);

    const TArray<UWidget*>& BuffIcons = HB_Buff->GetAllChildren();
    int32 IconIndex = 0;
    for (UWidget* BuffIcon : BuffIcons)
    {
        if (IconIndex >= CurrentBuffState.Num())
        {
            BuffIcon->SetRenderOpacity(0.f);
            continue;
        }

        UGBDurationIconWidget* BuffIWidget = Cast<UGBDurationIconWidget>(BuffIcon);
        if (BuffIWidget)
        {
            if (BuffIconMap.Contains(CurrentBuffState[IconIndex]) == false)
            {
                const UEnum* EnumPtr = StaticEnum<EGBBuffType>();
                if (EnumPtr)
                {
                    GB_LOG(TEXT("%s"), *EnumPtr->GetNameStringByValue(static_cast<int64>(CurrentBuffState[IconIndex])));
                }
                continue;
            }

            BuffIWidget->SetIconImage(BuffIconMap[CurrentBuffState[IconIndex]]);
            BuffIWidget->SetRenderOpacity(1.f);
        }

        ++IconIndex;
    }

    UpdateBuffRemainInfo();
}

void UGBPlayerHUDWidget::UpdateProgressbar(UProgressBar* Progressbar, const float Value)
{
    GB_NULL_CHECK(Progressbar);
    GB_CONDITION_CHECK(Value <= 0.f && Value <= 1.f);

    Progressbar->SetPercent(Value);
}

void UGBPlayerHUDWidget::UpdateBuffRemainInfo()
{
    GB_VALID_CHECK(AbilitySystemComponent);
    if (CurrentBuffState.IsEmpty())
    {
        return;
    }

    const int32 BuffCount = CurrentBuffState.Num();
    const int32 BuffWidgetCount = HB_Buff->GetChildrenCount();
    for (int32 BuffIndex = 0; BuffIndex < BuffCount; ++BuffIndex)
    {
        if (BuffIndex >= BuffWidgetCount)
        {
            break;
        }

        UWidget* BuffIcon = HB_Buff->GetChildAt(BuffIndex);
        UGBDurationIconWidget* BuffIWidget = Cast<UGBDurationIconWidget>(BuffIcon);
        GB_NULL_CHECK(BuffIWidget);

        const FGameplayTag BuffTag = GetTagByBuffType(CurrentBuffState[BuffIndex]);
        if (BuffTag.IsValid())
        {
            float RemainingTime = 0.f, TotalDuration = 0.f;
            AbilitySystemComponent->GetGameplayEffectDurationByAssetTag(BuffTag, RemainingTime, TotalDuration);

            BuffIWidget->SetDurationInfo(RemainingTime, TotalDuration);
        }
    }

    BuffUpdateTimerHandle.Invalidate();
    GetWorld()->GetTimerManager().SetTimer(BuffUpdateTimerHandle, this, &UGBPlayerHUDWidget::UpdateBuffRemainInfo, 0.1f, false);
}

void UGBPlayerHUDWidget::UpdateSkillRemainInfo()
{
    GB_VALID_CHECK(AbilitySystemComponent);
    if (SkillNumbersOnCooldown.IsEmpty())
    {
        return;
    }

    int32 SkillCount = SkillNumbersOnCooldown.Num();
    for (int32 SkillIndex = 0; SkillIndex < SkillCount; ++SkillIndex)
    {
        const int32 SkillNum = SkillNumbersOnCooldown[SkillIndex];
        UWidget* SkillIconWidget = HB_Skill->GetChildAt(SkillNum - 1);
        UGBDurationIconWidget* SkillWidget = Cast<UGBDurationIconWidget>(SkillIconWidget);
        GB_NULL_CHECK(SkillWidget);

        const FGameplayTag SkillTag = GetSkillTagByIndex(SkillNum);
        if (SkillTag.IsValid())
        {
            float RemainingTime = 0.f, TotalDuration = 0.f;
            AbilitySystemComponent->GetAbilityCoolTimeByAssetTag(SkillTag, RemainingTime, TotalDuration);
            SkillWidget->SetDurationInfo(RemainingTime, TotalDuration);
        }
    }

    SkillUpdateTimerHandle.Invalidate();
    GetWorld()->GetTimerManager().SetTimer(SkillUpdateTimerHandle, this, &UGBPlayerHUDWidget::UpdateSkillRemainInfo, 0.1f, false);
}

void UGBPlayerHUDWidget::UpdateSkillIconImage()
{
    const TArray<UWidget*>& SkillChilds = HB_Skill->GetAllChildren();

    if (IsValid(AbilitySystemComponent) && AbilitySystemComponent->HasMatchingGameplayTag(GBTag::State_Combat_Equipped))
    {
        int32 IconIndex = 1;
        for (UWidget* ChildWidget : SkillChilds)
        {
            UGBDurationIconWidget* IconWidget = Cast<UGBDurationIconWidget>(ChildWidget);
            if (IconWidget)
            {
                const FGameplayTag& SkillTag = GetSkillTagByIndex(IconIndex);
                if(SkillTag.IsValid() == false || SkillIconMap.Contains(SkillTag) == false)
                {
                    continue;
                }

                if (SkillIconMap[SkillTag])
                {
                    IconWidget->SetRenderOpacity(1.f);
                    IconWidget->SetIconImage(SkillIconMap[SkillTag]);
                    IconWidget->SetIconImageColorAndOpacity(FLinearColor::White);
                }
            }

            ++IconIndex;
        }
    }
    else
    {
        for (UWidget* ChildWidget : SkillChilds)
        {
            if (UGBDurationIconWidget* IconWidget = Cast<UGBDurationIconWidget>(ChildWidget))
            {
                IconWidget->SetRenderOpacity(0.3f);
                IconWidget->SetIconImageColorAndOpacity(FLinearColor::Black);
            }
        }
    }
}

void UGBPlayerHUDWidget::SetSkillIconTexture(const TArray<TPair<FGameplayTag, UTexture2D*>>& Texturelist)
{
    for(const TPair<FGameplayTag, UTexture2D*> pair : Texturelist)
    {
        if (SkillIconMap.Contains(pair.Key))
        {
            SkillIconMap[pair.Key] = pair.Value;
        }
        else
        {
            SkillIconMap.Add(pair.Key, pair.Value);
        }
    }
}

void UGBPlayerHUDWidget::OnSkillCooldownChanged(const FGameplayTag Tag, const int32 Count)
{
    GB_VALID_CHECK(AbilitySystemComponent);

    int32 SkillIndex = 0;
    GB_CONDITION_CHECK(GetIndexByCooldownTag(Tag, SkillIndex));
    GB_CONDITION_CHECK(SkillIndex != 0);
    GB_CONDITION_CHECK(SkillIndex <= HB_Skill->GetChildrenCount());

    UWidget* SkillIcon = HB_Skill->GetChildAt(SkillIndex - 1);
    UGBDurationIconWidget* SkillWidget = Cast<UGBDurationIconWidget>(SkillIcon);
    GB_NULL_CHECK(SkillWidget);

    const bool bTagAdded = Count > 0;
    if (bTagAdded)
    {
        if (SkillNumbersOnCooldown.Contains(SkillIndex) == false)
        {
            SkillNumbersOnCooldown.Add(SkillIndex);
            SkillWidget->SetRemainTextVisible(true);
        }
    }
    else
    {
        if (SkillNumbersOnCooldown.Contains(SkillIndex))
        {
            SkillNumbersOnCooldown.Remove(SkillIndex);
            SkillWidget->SetRemainTextVisible(false);
        }
    }

    if (SkillNumbersOnCooldown.IsEmpty())
    {
        SkillUpdateTimerHandle.Invalidate();
    }
    else
    {
        UpdateSkillRemainInfo();
    }
}

void UGBPlayerHUDWidget::OnSkillInfoChanged(const FGameplayTag Tag, const int32 Count)
{
    UpdateSkillIconImage();
}

EGBBuffType UGBPlayerHUDWidget::GetBuffTypeByTag(const FGameplayTag Tag) const
{
    if (Tag == GBTag::State_Buff_Attack)
    {
        return EGBBuffType::AttackUp;
    }
    else if (Tag == GBTag::State_Buff_Defense)
    {
        return EGBBuffType::DefenseUp;
    }
    else if (Tag == GBTag::State_Buff_Speed)
    {
        return EGBBuffType::SpeedUp;
    }
    else if (Tag == GBTag::State_Buff_Heal)
    {
        return EGBBuffType::Heal;
    }
    else if (Tag == GBTag::State_Debuff_Fire)
    {
        return EGBBuffType::Fire;
    }
    else if (Tag == GBTag::State_Debuff_Poison)
    {
        return EGBBuffType::Poison;
    }
    else if (Tag == GBTag::State_Debuff_Stun)
    {
        return EGBBuffType::Stun;
    }
    else if (Tag == GBTag::State_Buff_Invincible)
    {
        return EGBBuffType::Invincible;
    }
    else if (Tag == GBTag::State_Debuff_Frozen)
    {
        return EGBBuffType::Frozen;
    }
    else
    {
        return EGBBuffType::None;
    }
}

FGameplayTag UGBPlayerHUDWidget::GetTagByBuffType(const EGBBuffType BuffType) const
{
    if (BuffType == EGBBuffType::AttackUp)
    {
        return GBTag::State_Buff_Attack;
    }
    else if (BuffType == EGBBuffType::DefenseUp)
    {
        return GBTag::State_Buff_Defense;
    }
    else if (BuffType == EGBBuffType::SpeedUp)
    {
        return GBTag::State_Buff_Speed;
    }
    else if (BuffType == EGBBuffType::Heal)
    {
        return GBTag::State_Buff_Heal;
    }
    else if (BuffType == EGBBuffType::Fire)
    {
        return GBTag::State_Debuff_Fire;
    }
    else if (BuffType == EGBBuffType::Poison)
    {
        return GBTag::State_Debuff_Poison;
    }
    else if (BuffType == EGBBuffType::Stun)
    {
        return GBTag::State_Debuff_Stun;
    }
    else if (BuffType == EGBBuffType::Invincible)
    {
        return GBTag::State_Buff_Invincible;
    }
    else if (BuffType == EGBBuffType::Frozen)
    {
        return GBTag::State_Debuff_Frozen;
    }
    else
    {
        return FGameplayTag::EmptyTag;
    }
}

FGameplayTag UGBPlayerHUDWidget::GetSkillTagByIndex(const int32 Index) const
{
    switch (Index)
    {
    case 1:
        return GBTag::Trigger_Common_Skill01;
        break;
    case 2:
        return GBTag::Trigger_Common_Skill02;
        break;
    case 3:
        return GBTag::Trigger_Common_Skill03;
        break;
    case 4:
        return GBTag::Trigger_Common_Skill04;
        break;
    }

    return FGameplayTag::EmptyTag;
}

bool UGBPlayerHUDWidget::GetIndexByCooldownTag(const FGameplayTag CooldownTag, int32& OutIndex) const
{
    OutIndex = 0;

    if (CooldownTag == GBTag::State_Cooldown_Skill01)
    {
        OutIndex = 1;
    }
    else if (CooldownTag == GBTag::State_Cooldown_Skill02)
    {
        OutIndex = 2;
    }
    else if (CooldownTag == GBTag::State_Cooldown_Skill03)
    {
        OutIndex = 3;
    }
    else if (CooldownTag == GBTag::State_Cooldown_Skill04)
    {
        OutIndex = 4;
    }

    return (OutIndex != 0);

}
