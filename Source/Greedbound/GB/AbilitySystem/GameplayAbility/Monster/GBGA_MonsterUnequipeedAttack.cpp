#include "GBGA_MonsterUnequipeedAttack.h"
#include "Define/GBTags.h"

FOnAbilityEndedDelegate* UGBGA_MonsterUnequipeedAttack::GetOnAbilityEndedDelegate()
{
    return &OnAbilityEnded;
}

void UGBGA_MonsterUnequipeedAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

    OnAbilityEnded.Broadcast();
    OnAbilityEnded.Clear();
}

void UGBGA_MonsterUnequipeedAttack::OnEventReceived(const FGameplayTag EventTag, FGameplayEventData EventData)
{
    Super::OnEventReceived(EventTag, EventData);

    if (EventTag == GBTag::Event_Common_Attack)
    {
        // 발차기가 구려서 2단 콤보만 하도록 함.
        if (CurrentComboIndex < 2)
        {
            SetContinueCombo(true);
        }
    }
}
