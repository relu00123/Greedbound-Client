// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GameplayTagContainer.h"
#include "GBAT_PlayMontageAndWaitForEvent.generated.h"

class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGBPlayMontageAndWaitForEventDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);

UCLASS()
class GREEDBOUND_API UGBAT_PlayMontageAndWaitForEvent : public UAbilityTask
{
    GENERATED_BODY()
    
public:
    virtual void                            Activate() override final;
    virtual void                            OnDestroy(bool AbilityEnded) override final;
    
    UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
    static UGBAT_PlayMontageAndWaitForEvent* PlayMontageAndWaitForEvent(UGameplayAbility* OwningAbility,
                                            const FName TaskInstanceName,
                                            UAnimMontage* MontageToPlay,
                                            FGameplayTagContainer EventTags,
                                            float Rate = 1.f,
                                            const FName StartSection = NAME_None,
                                            const bool bStopWhenAbilityEnds = true,
                                            const float AnimRootMotionTranslationScale = 1.f);

    const bool                              StopPlayingMontage();
    void                                    JumpToSection(const FName SectionName);
    void                                    OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
    void                                    OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload);

public:
    UPROPERTY(BlueprintAssignable)
    FGBPlayMontageAndWaitForEventDelegate   OnCompleted;
    
    UPROPERTY(BlueprintAssignable)
    FGBPlayMontageAndWaitForEventDelegate   OnEventReceived;
    
    UPROPERTY()
    TObjectPtr<UAnimMontage>                MontageToPlay;

    UPROPERTY()
    FGameplayTagContainer                   EventTags;

    UPROPERTY()
    float                                   Rate;

    UPROPERTY()
    FName                                   StartSection;

    UPROPERTY()
    float                                   AnimRootMotionTranslationScale;

    UPROPERTY()
    bool                                    bStopWhenAbilityEnds;

    FOnMontageEnded                         MontageEndedDelegate;
    FDelegateHandle                         EventHandle;
};
