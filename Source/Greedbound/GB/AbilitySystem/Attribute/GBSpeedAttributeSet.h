// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GBSpeedAttributeSet.generated.h"

class FLifetimeProperty;
struct FGameplayEffectModCallbackData;

UCLASS()
class GREEDBOUND_API UGBSpeedAttributeSet : public UAttributeSet
{
    GENERATED_BODY()
    
public:
    ATTRIBUTE_ACCESSORS_BASIC(UGBSpeedAttributeSet, CurrentSpeed);
    ATTRIBUTE_ACCESSORS_BASIC(UGBSpeedAttributeSet, WalkSpeed);
    ATTRIBUTE_ACCESSORS_BASIC(UGBSpeedAttributeSet, SprintSpeed);
    ATTRIBUTE_ACCESSORS_BASIC(UGBSpeedAttributeSet, CurrnetStamina);
    ATTRIBUTE_ACCESSORS_BASIC(UGBSpeedAttributeSet, MaxStamina);
    ATTRIBUTE_ACCESSORS_BASIC(UGBSpeedAttributeSet, StaminaCost);
    ATTRIBUTE_ACCESSORS_BASIC(UGBSpeedAttributeSet, SprintStaminaThreshould);
    ATTRIBUTE_ACCESSORS_BASIC(UGBSpeedAttributeSet, CrouchSpeedCoefficient);

    // 아래 4개 함수는 부모에서 구현부가 없는 함수이므로 Super 호출하지 말 것
    //virtual bool            PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override final;
    //virtual void            PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override final {};
    virtual void            PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override final;
    virtual void            PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override final;

    virtual void            GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override final;
    
private:
    UFUNCTION()
    void                    OnRep_CurrnetSpeed(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void                    OnRep_CurrnetStamina(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void                    OnRep_MaxStamina(const FGameplayAttributeData& OldValue);

public:
    UPROPERTY(BlueprintReadOnly, Category = "Speed", Replicated)
    FGameplayAttributeData  CurrentSpeed;

    UPROPERTY(BlueprintReadOnly, Category = "Speed", Replicated)
    FGameplayAttributeData  WalkSpeed;
    
    UPROPERTY(BlueprintReadOnly, Category = "Speed", Replicated)
    FGameplayAttributeData  SprintSpeed;
    
    UPROPERTY(BlueprintReadOnly, Category = "Speed", ReplicatedUsing = OnRep_CurrnetStamina)
    FGameplayAttributeData  CurrnetStamina;
    
    UPROPERTY(BlueprintReadOnly, Category = "Speed", ReplicatedUsing = OnRep_MaxStamina)
    FGameplayAttributeData  MaxStamina;

    UPROPERTY(BlueprintReadOnly, Category = "Speed", Replicated)
    FGameplayAttributeData  StaminaCost;
    
    UPROPERTY(BlueprintReadOnly, Category = "Speed", Replicated)
    FGameplayAttributeData  SprintStaminaThreshould;

    UPROPERTY(BlueprintReadOnly, Category = "Speed", Replicated)
    FGameplayAttributeData  CrouchSpeedCoefficient;

};
