// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GB/AbilitySystem/GameplayAbility/GBGA_Base.h"
#include "GBGA_ChannelingBase.generated.h"

class UGBAT_WaitChanneling;

// 공통 Channeling 기반 GA Base
// 공통 Flow :
// - ActivateAbility()
//   - UGBAT_WaitChanneling Task 시작
//   - BP : K2_OnChannelingStarted() 호출
// - 채널링 성공
//   - HandleChannelSuccess()
//   - BP : K2_OnChannelingSucceeded()
//   - (옵션) commitAbility() 호출
//   - EndAbility(bWasCanceled = false)
// - 채널링 취소/인터럽트
//   - HandleChannelCanceled()
//   - BP : K2_OnChannelingCanceled()
//   - EndAbility(bWasCanceled = true)

UCLASS(Abstract)
class GREEDBOUND_API UGBGA_ChannelingBase : public UGBGA_Base
{
	GENERATED_BODY()
	
public:
    UGBGA_ChannelingBase();

protected:
    /* === UGameplayAbility 기본 오버라이드 === */

    virtual void                    ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo  ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void                    EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo  ActivationInfo, bool bReplicateEndAbility, bool bWasCanceled) override;

    /* === 확장 포인트 === */

    /**
     * 채널링을 시작해도 되는지 확인
     *
     * 기본 동작:
     * - Ability에 CostGE가 있으면 CheckCost() 자동 처리
     * - Ability에 CooldownGE가 있으면 CheckCooldown() 자동 처리
     * - 둘 다 없으면 그대로 true
     *
     * 자식 Ability 필요 시 override 가능
     */
    virtual bool                    CanStartChanneling(const FGameplayAbilitySpecHandle& Handle, const FGameplayAbilityActorInfo* ActorInfo) const;
        
    /* === AbilityTask 콜백 === */

    UFUNCTION()
    void                            HandleChannelSuccess();

    UFUNCTION()
    void                            HandleChannelCanceled();

    /** 채널링이 시작될 때 (Task ReadyForActivation 직후) */
    UFUNCTION(BlueprintImplementableEvent, Category = "Channeling")
    void                            K2_OnChannelingStarted();

    /** 채널링이 정상적으로 끝났을 때 (Commit/EndAbility 직전에 호출) */
    UFUNCTION(BlueprintImplementableEvent, Category = "Channeling")
    void                            K2_OnChannelingSucceeded();

    /** 채널링이 중간에 취소/인터럽트 되었을 때 (EndAbility 직전에 호출) */
    UFUNCTION(BlueprintImplementableEvent, Category = "Channeling")
    void                            K2_OnChannelingCanceled();

protected:
    /** 현재 진행 중인 채널링 Task (없을 수도 있음) */
    UPROPERTY()
    TObjectPtr<UGBAT_WaitChanneling>            ActiveChannelTask;

    // Channeling 설정 값들 (BP에서 수정 가능)

    // 채널링 유지 시간(초)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Channeling")
    float                                       ChannelTime;

    // 채널링 중에 ASC에 붙일 테그 ( ex : State.Action.Channeling) 
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Channeling")
    FGameplayTag                                ChannelingTag;

    // 붙으면 채널링을 취소시키는 태그들 ( ex : State.Stunned 등)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Channeling")
    FGameplayTagContainer                       CancelTags;
    
	// 채널링 성공 시 Commit Ability(=쿨타임/코스트 적용)를 할지 여부
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Channeling")
    bool                                        bCommitOnSuccess;
};
