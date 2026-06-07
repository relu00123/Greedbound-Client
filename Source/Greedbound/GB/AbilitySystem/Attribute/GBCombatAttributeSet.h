// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GBCombatAttributeSet.generated.h"

class FLifetimeProperty;
struct FGameplayEffectModCallbackData;

UCLASS()
class GREEDBOUND_API UGBCombatAttributeSet : public UAttributeSet
{
    GENERATED_BODY()
    
public:
    ATTRIBUTE_ACCESSORS_BASIC(UGBCombatAttributeSet, Attack);
    ATTRIBUTE_ACCESSORS_BASIC(UGBCombatAttributeSet, Defense);

    // 아래 4개 함수는 부모에서 구현부가 없는 함수이므로 Super 호출하지 말 것
    //virtual bool            PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override final {};
    //virtual void            PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override final {};
    //virtual void            PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override final {};
    //virtual void            PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override final {};

    virtual void            GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override final;
    
    UPROPERTY(BlueprintReadOnly, Category = "Combat", Replicated)
    FGameplayAttributeData  Attack;
    
    UPROPERTY(BlueprintReadOnly, Category = "Combat", Replicated)
    FGameplayAttributeData  Defense;
};
