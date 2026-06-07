#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/Common/GBGA_UnequippedAttack.h"
#include "Interface/GAS/GBEndAbilityBindInterface.h"
#include "GBGA_MonsterUnequipeedAttack.generated.h"

UCLASS()
class GREEDBOUND_API UGBGA_MonsterUnequipeedAttack : public UGBGA_UnequippedAttack, public IGBEndAbilityBindInterface
{
    GENERATED_BODY()

public:
    virtual FOnAbilityEndedDelegate*    GetOnAbilityEndedDelegate() override final;

protected:
    virtual void                        EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override final;

    virtual void                        OnEventReceived(const FGameplayTag EventTag, FGameplayEventData EventData) override final;

private:
    FOnAbilityEndedDelegate             OnAbilityEnded;
};
