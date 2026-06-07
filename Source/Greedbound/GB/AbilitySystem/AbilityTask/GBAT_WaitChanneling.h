// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GameplayTagContainer.h"
#include "GBAT_WaitChanneling.generated.h"

class UAbilitySystemComponent; 

// 채널링 성공/취소 알림 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGBChannelingDelegate);

// Channeling을 기다리는 AbilityTask
// 일정 시간 동안 유지되면 성공
// CancelTag가 붙거나 Ability가 끝나면 취소로 간주
// ChannelingTag를 시작 시 ASC에 추가, 종료시 제거 
UCLASS()
class GREEDBOUND_API UGBAT_WaitChanneling : public UAbilityTask
{
	GENERATED_BODY()
	
public:
    UGBAT_WaitChanneling();

    // 채널링이 정상적으로 끝났을 때 브로드캐스트
    UPROPERTY(BlueprintAssignable)
    FGBChannelingDelegate                       OnChannelSuccess;

    // 채널링이 중간에 취소/중단되었을 때 브로드캐스트
    UPROPERTY(BlueprintAssignable)
    FGBChannelingDelegate                       OnChannelCanceled;

    // Channeling을 시작하교 결과를 기다리는 Task 생성
    // @param OwningAbility    : 이 Task를 소유하는 Ability 
    // @param TaskInstanceName : 여러 Task를 동시에 쓸 때 구분용 이름(보통은 NAME_None)
    // @param InChannelTime    : 채널링 유지 시간 (초)
    // @param InChannelingTag  : 채널링 상태를 나타낼 GameplayTag (선택)
    // @param InCancelTags     : 붙으면 채널링을 취소시킬 Tag들 (선택) 
    // 블루프린트에서 일반적으로 노출 x. 사용자가 직접 그래프에 드래그해 놓고 쓰는 것 방지
    UFUNCTION(BlueprintCallable, Category = "Greedbound|Ability|Tasks", meta = (DisplayName = "Wait Channeling", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))  
    static UGBAT_WaitChanneling*                WaitChanneling(
                                                    UGameplayAbility* OwningAbility,
                                                    FName              TaskInstanceName,
                                                    float              InChannelTime,
                                                    FGameplayTag       InChannelingTag,
                                                    FGameplayTagContainer InCancelTags);

protected :
    // UAbilityTask 인터페이스
    virtual void                                Activate() override; 
    virtual void                                OnDestroy(bool bInOwnerFinished) override;

    // 타이머가 끝났을 때 (성공)
    void                                        HandleTimerFinished();

    // Cancel Tag 변화 감지 
    void                                        HandleCancelTagChanged(FGameplayTag Tag, int32 NewCount);

    // 성공/취소 처리 공통 로직
    void                                        FinishSuccess();
    void                                        FinishCanceled();

    // ASC에서 ChannelingTag 제거 및 델레게이트 언바인드 등 정리
    void                                        Cleanup(); 
                
protected :
    // 채널링 유지 시간
    float                                       ChannelTime = 0.f; 

    /** 채널링 중일 때 ASC 에 붙일 Tag (예: State.Channeling) */
    FGameplayTag                                ChannelingTag;

    /** 채널링을 깨트리는 Tag 들 (예: State.Stunned, State.Knockback 등) */
    FGameplayTagContainer                       CancelTags;

    /** 내부 타이머 핸들 */
    FTimerHandle                                TimerHandle;

    /** 캐시된 ASC 포인터 */
    TWeakObjectPtr<UAbilitySystemComponent>     CachedASC;

    /** CancelTag 이벤트 바인딩 핸들들 */
    TArray<FDelegateHandle>                     CancelTagDelegateHandles;

    /** 한번만 브로드캐스트하기 위한 플래그 */
    uint8                                       bFinishedSuccess : 1 = false;
    uint8                                       bFinishedCanceled : 1 = false;
};
