// Fill out your copyright notice in the Description page of Project Settings.

#include "GBMonsterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/Monster/GBMonsterBehaviorComponent.h"
#include "Components/GBAbilitySystemComponent.h"
#include "Components/GBMovementStateComponent.h"
#include "Components/GBCombatComponent.h"
#include "Define/GBDefine.h"
#include "Define/GBTags.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "ProfilingDebugging/CpuProfilerTrace.h"

AGBMonsterCharacter::AGBMonsterCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    UGBAbilitySystemComponent* ASC = CreateDefaultSubobject<UGBAbilitySystemComponent>("AbilitySystem");
    ASC->SetIsReplicated(true);
    ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
    SetAbilitySystemComponent(ASC);

    BehaviorComponent = CreateDefaultSubobject<UGBMonsterBehaviorComponent>(TEXT("BehaviorComponent"));

    if (IGBCombatInterface::Execute_GetCombatComponent(this))
    {
        IGBCombatInterface::Execute_GetCombatComponent(this)->SetAttackProfile(GBCOLLISION_PROFILE_MONSTERAOE);
    }
}

void AGBMonsterCharacter::BeginPlay()
{
    Super::BeginPlay();

    InitAbilitySystemComponent(this);
}

void AGBMonsterCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
}

void AGBMonsterCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateStopState();
}
// 
// void AGBMonsterCharacter::OnMovementStateTagChanged(const FGameplayTag Tag, const int32 Count)
// {
//     Super::OnMovementStateTagChanged(Tag, Count);
//     
//     GB_CONDITION_CHECK_WITHOUT_LOG(HasAuthority());
// 
//     if (Tag == GBTag::Block_All || Tag == GBTag::Block_Move)
//     {
//         UCharacterMovementComponent* Move = GetCharacterMovement();
//         GB_NULL_CHECK(Move);
// 
//         if (Count > 0)
//         {
//             Move->DisableMovement();
//         }
//         else
//         {
//             Move->SetMovementMode(MOVE_Walking);
// 
//             // 비용이 꽤 크다고 한다. 임시 코드이다. 나중에 Decorator 로 Abort 하는 방식으로 수정해야 한다. 
//             if (AAIController* AIC = Cast<AAIController>(GetController()))
//             {
//                 TRACE_CPUPROFILER_EVENT_SCOPE(GB_BT_RestartLogic);  // Insights에서 이 이름으로 검색 가능
//                 QUICK_SCOPE_CYCLE_COUNTER(STAT_GB_BT_RestartLogic); // Stat 이벤트(백업용)
// 
//                 if (UBrainComponent* Brain = AIC->GetBrainComponent())
//                 {
//                     Brain->RestartLogic();
//                 }
//             }
//         }
//     }
// }

UGBMonsterBaseData* AGBMonsterCharacter::GetMonsterBaseData_Implementation() const
{
    return BehaviorComponent->GetMonsterBaseData();
}

void AGBMonsterCharacter::UpdateStopState()
{
    UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
    GB_NULL_CHECK(CharacterMovementComponent);

    const FVector Velocity = GetVelocity();
    const float GroundSpeed = Velocity.Size2D();

    const bool bIsMoving = GroundSpeed > GBMovementConstants::MoveThreshold;

    //SetCharacterStopState(bIsMoving ? EGBStopState::Move : EGBStopState::Stop);
}
