#include "GBGA_Sprint.h"
#include "AbilitySystem/Attribute/GBSpeedAttributeSet.h"
#include "Interface/GBMovementInterface.h"
#include "Components/GBMovementStateComponent.h"
#include "Define/GBDefine.h"
#include "Define/GBTags.h"

constexpr float DelayToRegenStamina = 1.f;

UGBGA_Sprint::UGBGA_Sprint(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGBGA_Sprint::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
    if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) == false)
    {
        return false;
    }

    const FGameplayAttribute& CurrentStaminaAttribute = UGBSpeedAttributeSet::GetCurrnetStaminaAttribute();
    const FGameplayAttribute& SprintThreshouldAttribute = UGBSpeedAttributeSet::GetSprintStaminaThreshouldAttribute();
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (ASC && CurrentStaminaAttribute.IsValid() && SprintThreshouldAttribute.IsValid())
    {
        const float CurrnetStamina = ASC->GetNumericAttribute(CurrentStaminaAttribute);
        const float SprintThreshould = ASC->GetNumericAttribute(SprintThreshouldAttribute);
        if (SprintThreshould < CurrnetStamina)
        {
            return true;
        }
    }

    return false;
}

void UGBGA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
    {
        return;
    }

    if (MovementStateComponent == nullptr)
    {
        MovementStateComponent = IGBMovementInterface::Execute_GetMovementStateComponent(ActorInfo->AvatarActor.Get());
    }

    GB_NULL_CHECK(MovementStateComponent);
    MovementStateComponent->SetMoveState(EGBMoveState::Sprint);

    UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
    if (ASC)
    {
        const float SprintSpeed = ASC->GetNumericAttribute(UGBSpeedAttributeSet::GetSprintSpeedAttribute());
        ASC->SetNumericAttributeBase(UGBSpeedAttributeSet::GetCurrentSpeedAttribute(), SprintSpeed);
    }

    if (RegenStaminaTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(RegenStaminaTimer);
    }
}

void UGBGA_Sprint::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    Super::InputReleased(Handle, ActorInfo, ActivationInfo);

    const bool bReplicatedEndAbility = true;
    const bool bWasCancelled = true;
    EndAbility(Handle, ActorInfo, ActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UGBGA_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    GB_NULL_CHECK(MovementStateComponent);
    MovementStateComponent->SetMoveState(EGBMoveState::Walk);

    UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
    if (ASC)
    {
        const float WalkSpeed = ASC->GetNumericAttribute(UGBSpeedAttributeSet::GetWalkSpeedAttribute());
        ASC->SetNumericAttributeBase(UGBSpeedAttributeSet::GetCurrentSpeedAttribute(), WalkSpeed);
    }

    RemoveActiveEffectsByGrantedTag(GBTag::Duration_Cost_Stamina);

    // DelayToRegenStamina 이후 RegenStamina GA Event 발동
    FTimerDelegate LamdaDelegate = FTimerDelegate::CreateLambda([ASC]() 
        {
            if (ASC)
            {
                FGameplayEventData EventData;
                EventData.EventTag = GBTag::Trigger_Common_RegenStamina;
                EventData.Instigator = ASC->GetAvatarActor();

                ASC->HandleGameplayEvent(GBTag::Trigger_Common_RegenStamina, &EventData);
            }
        }
    );

    GetWorld()->GetTimerManager().SetTimer(RegenStaminaTimer, LamdaDelegate, DelayToRegenStamina, false);

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
