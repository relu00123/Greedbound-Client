// Fill out your copyright notice in the Description page of Project Settings.


#include "GBAT_WaitChanneling.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Abilities/GameplayAbility.h"

UGBAT_WaitChanneling::UGBAT_WaitChanneling()
{
    bTickingTask = false; // 틱 필요 없음 (타이머 기반)
}

UGBAT_WaitChanneling* UGBAT_WaitChanneling::WaitChanneling(
    UGameplayAbility* OwningAbility,
    FName                   TaskInstanceName,
    float                   InChannelTime,
    FGameplayTag            InChannelingTag,
    FGameplayTagContainer   InCancelTags)
{
    UGBAT_WaitChanneling* MyTask = NewAbilityTask<UGBAT_WaitChanneling>(OwningAbility, TaskInstanceName);
    MyTask->ChannelTime = InChannelTime;
    MyTask->ChannelingTag = InChannelingTag;
    MyTask->CancelTags = InCancelTags;
    return MyTask;
}

void UGBAT_WaitChanneling::Activate()
{
    if (!Ability)
    {
        EndTask();
        return;
    }

    UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
    CachedASC = ASC;

    // Channeling Tag 부여
    if (ASC && ChannelingTag.IsValid())
    {
        ASC->AddLooseGameplayTag(ChannelingTag);
    }

    // CancelTag 이벤트 등록
    if (ASC && CancelTags.Num() > 0)
    {
        for (const FGameplayTag& Tag : CancelTags)
        {
            FDelegateHandle Handle = ASC
                ->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved)
                .AddUObject(this, &UGBAT_WaitChanneling::HandleCancelTagChanged);

            CancelTagDelegateHandles.Add(Handle);
        }
    }

    // 타이머 시작
    UWorld* World = GetWorld();
    if (!World || ChannelTime <= 0.f)
    {
        // 시간이 0 이하라면 바로 성공 처리
        FinishSuccess();
        return;
    }

    World->GetTimerManager().SetTimer(
        TimerHandle,
        this,
        &UGBAT_WaitChanneling::HandleTimerFinished,
        ChannelTime,
        false
    );
}

void UGBAT_WaitChanneling::HandleTimerFinished()
{
    if (bFinishedSuccess || bFinishedCanceled)
    {
        return;
    }

    FinishSuccess();
}

void UGBAT_WaitChanneling::HandleCancelTagChanged(FGameplayTag Tag, int32 NewCount)
{
    // CancelTag 가 0 -> 1 로 바뀌는 순간을 Cancel 로 취급
    if (NewCount > 0 && !bFinishedSuccess && !bFinishedCanceled)
    {
        FinishCanceled();
    }
}

void UGBAT_WaitChanneling::FinishSuccess()
{
    if (bFinishedSuccess || bFinishedCanceled)
    {
        return;
    }

    bFinishedSuccess = true;

    Cleanup();

    if (ShouldBroadcastAbilityTaskDelegates())
    {
        OnChannelSuccess.Broadcast();
    }

    EndTask();
}

void UGBAT_WaitChanneling::FinishCanceled()
{
    if (bFinishedSuccess || bFinishedCanceled)
    {
        return;
    }

    bFinishedCanceled = true;

    Cleanup();

    if (ShouldBroadcastAbilityTaskDelegates())
    {
        OnChannelCanceled.Broadcast();
    }

    EndTask();
}

void UGBAT_WaitChanneling::Cleanup()
{
    // 타이머 제거
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(TimerHandle);
    }

    // CancelTag 이벤트 언바인드
    if (UAbilitySystemComponent* ASC = CachedASC.Get())
    {
        //  FGameplayTagContainer → TArray<FGameplayTag>로 변환
        TArray<FGameplayTag> CancelTagArray;
        CancelTags.GetGameplayTagArray(CancelTagArray);

        for (int32 i = 0; i < CancelTagDelegateHandles.Num(); ++i)
        {
            if (CancelTagArray.IsValidIndex(i))
            {
                const FGameplayTag Tag = CancelTagArray[i];

                ASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved)
                    .Remove(CancelTagDelegateHandles[i]);
            }
        }

        CancelTagDelegateHandles.Empty();

        // ChannelingTag 제거
        if (ChannelingTag.IsValid() && ASC->HasMatchingGameplayTag(ChannelingTag))
        {
            ASC->RemoveLooseGameplayTag(ChannelingTag);
        }
    }
}

void UGBAT_WaitChanneling::OnDestroy(bool bInOwnerFinished)
{
    // Ability 가 강제로 끝났는데 (Cancel 등)
    // 성공/취소 처리도 아직 안 된 경우 → 취소로 간주
    if (!bFinishedSuccess && !bFinishedCanceled)
    {
        FinishCanceled(); // 여기서 Cleanup + OnChannelCanceled + EndTask 처리됨
        // FinishCanceled 안에서 EndTask 호출하므로 여기서 다시 EndTask() 호출할 필요 없음
        return;
    }

    // 이미 FinishSuccess/FinishCanceled 에서 EndTask 까지 호출된 경우
    Super::OnDestroy(bInOwnerFinished);
}



