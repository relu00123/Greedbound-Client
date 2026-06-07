// Fill out your copyright notice in the Description page of Project Settings.


#include "GBGA_ChannelingBase.h"
#include "AbilitySystem/AbilityTask/GBAT_WaitChanneling.h"

UGBGA_ChannelingBase::UGBGA_ChannelingBase()
{
    // 기본값 (BP에서 덮어쓰기 가능)
    ChannelTime = 3.f;
    bCommitOnSuccess = true;

    // Channeling 류는 보통 인스턴스 기반으로 쓰는 게 편해서
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGBGA_ChannelingBase::CanStartChanneling(const FGameplayAbilitySpecHandle& Handle, const FGameplayAbilityActorInfo* ActorInfo) const 
{
    FGameplayTagContainer FailureTags;

    // Cost 검사 (Ability에 CostGE 없으면 true)
    if (!CheckCost(Handle, ActorInfo, &FailureTags))
    {
        return false;
    }

    // Cooldown 검사 (Ability에 CooldownGE 없으면 true)
    if (!CheckCooldown(Handle, ActorInfo, &FailureTags))
    {
        return false;
    }

    return true;
}


void UGBGA_ChannelingBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo  ActivationInfo, const FGameplayEventData* TriggerEventData) 
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    // 채널링 시작 조건 검사
    if (!CanStartChanneling(Handle, ActorInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // Channeling Task 생성
    ActiveChannelTask = UGBAT_WaitChanneling::WaitChanneling(
        this,
        NAME_None,
        ChannelTime,
        ChannelingTag,
        CancelTags
    );

    if (ActiveChannelTask)
    {
        // 성공/취소 델리게이트 바인딩
        ActiveChannelTask->OnChannelSuccess.AddDynamic(
            this, &UGBGA_ChannelingBase::HandleChannelSuccess);

        ActiveChannelTask->OnChannelCanceled.AddDynamic(
            this, &UGBGA_ChannelingBase::HandleChannelCanceled);

        // Task 실제 활성화
        ActiveChannelTask->ReadyForActivation();
    }

    // BP에서 캐스팅 시작 몽타주 등 연출을 넣을 수 있는 Hook
    K2_OnChannelingStarted();
}

void UGBGA_ChannelingBase::HandleChannelSuccess()
{
    ActiveChannelTask = nullptr;

    // BP: 실제 효과 (힐/딜, 상자열기, 탈출 등)
    K2_OnChannelingSucceeded();

    // 채널링 성공 시점에만 Cost/쿨타임 적용
    if (bCommitOnSuccess)
    {
        // UGameplayAbility 쪽 함수 (UGBGA_Base가 UGameplayAbility 상속 가정)
        CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
    }

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, /*bReplicateEndAbility*/ true, /*bWasCanceled*/ false);
}

void UGBGA_ChannelingBase::HandleChannelCanceled()
{
    ActiveChannelTask = nullptr;

    // BP: 실패 연출(캐스팅 취소, 몽타주 Stop 등)
    K2_OnChannelingCanceled();

    // 취소된 경우에는 기본적으로 쿨타임을 걸지 않고 끝낸다.
    // (원하면 여기서 별도 로직으로 "부분 쿨타임" 같은 것도 구현 가능)
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
        /*bReplicateEndAbility*/ true,
        /*bWasCanceled*/ true);
}

void UGBGA_ChannelingBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo  ActivationInfo, bool bReplicateEndAbility, bool bWasCanceled)
{
    // 혹시 Task 가 살아 있다면 안전하게 정리
    ActiveChannelTask = nullptr;

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCanceled);
}




