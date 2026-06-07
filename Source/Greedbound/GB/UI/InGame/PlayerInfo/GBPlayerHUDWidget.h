// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "GBPlayerHUDWidget.generated.h"

UENUM(BlueprintType)
enum class EGBBuffType : uint8
{
    None,
    Fire,
    Poison,
    Stun,
    AttackUp,
    DefenseUp,
    SpeedUp,
    Heal,
    Invincible,
    Frozen
};

class UTexture2D;
class UImage;
class UProgressBar;
class UHorizontalBox;
class UGBAbilitySystemComponent;
class UAbilitySystemComponent;
struct FOnAttributeChangeData;
struct FGameplayEventData;

UCLASS()
class GREEDBOUND_API UGBPlayerHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void                                        InitializeAttributeInfo(UAbilitySystemComponent* ASC);
    void                                        OnHealthAttributeChanged(const FOnAttributeChangeData& Payload);
    void                                        OnSpeedAttributeChanged(const FOnAttributeChangeData& Payload);
    void                                        OnBuffStateChanged(const FGameplayTag Tag, const int32 Count);
    void                                        OnSkillCooldownChanged(const FGameplayTag Tag, const int32 Count);
    void                                        OnSkillInfoChanged(const FGameplayTag Tag, const int32 Count);

    void                                        SetSkillIconTexture(const TArray<TPair<FGameplayTag, UTexture2D*>>& Texturelist);

private:
    void                                        UpdateProgressbar(UProgressBar* Progressbar, const float Value);
    void                                        UpdateBuffRemainInfo();
    void                                        UpdateSkillRemainInfo();
    void                                        UpdateSkillIconImage();

    EGBBuffType                                 GetBuffTypeByTag(const FGameplayTag Tag) const;
    FGameplayTag                                GetTagByBuffType(const EGBBuffType BuffType) const;
    FGameplayTag                                GetSkillTagByIndex(const int32 Index) const;
    bool                                        GetIndexByCooldownTag(const FGameplayTag CooldownTag, int32& OutIndex) const;

private:
    UPROPERTY()
    TObjectPtr<UGBAbilitySystemComponent>       AbilitySystemComponent;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UHorizontalBox>                  HB_Buff;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar>                    Pb_HPBar;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar>                    Pb_StaminaBar;
    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UHorizontalBox>                  HB_Skill;

    UPROPERTY(EditDefaultsOnly)
    TMap<EGBBuffType, TObjectPtr<UTexture2D>>   BuffIconMap;

    UPROPERTY()
    TMap<FGameplayTag, TObjectPtr<UTexture2D>>  SkillIconMap;

    TArray<EGBBuffType>                         CurrentBuffState;
    float                                       CurrentHealth   = 0.f;
    float                                       MaxHealth       = 0.f;
    float                                       CurrentStamina  = 0.f;
    float                                       MaxStamina      = 0.f;
    TArray<int32>                               SkillNumbersOnCooldown;

    FTimerHandle                                BuffUpdateTimerHandle;        
    FTimerHandle                                SkillUpdateTimerHandle;        
};
