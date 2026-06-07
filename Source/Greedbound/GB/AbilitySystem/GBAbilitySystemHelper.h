#pragma once

class UAbilitySystemComponent;
class UGameplayAbility;
class UGBGA_Base;
class UGBAbilitySystemComponent;
struct FGameplayAbilityActorInfo;
struct FGameplayAbilitySpec;
struct FGameplayEventData;
struct FGameplayTag;
struct FGameplayTagContainer;
enum class EGBTriggerType : uint8;

class FGBAbilitySystemHelper
{
public:
    static UGBAbilitySystemComponent*   GetAbilitySystemComponentFromActor(AActor* Actor);

    static void                         SendGameplayEventToTarget(const FGameplayTag EventTag, AActor* Instigator, AActor* Target, const float Magnitude = 0.f);
    static void                         SendGameplayEventToTarget(const FGameplayTag EventTag, AActor* Instigator, AActor* Target, UObject* Object);
    static void                         SendGameplayEventToSelf(const FGameplayTag EventTag, AActor* SourceActor, const float Magnitude = 0.f);
    static void                         SendGameplayEventToTarget(const FGameplayTag EventTag, AActor* SourceActor, UObject* Object);

    static bool                         HasMatchingGameplayTag(const FGameplayTag TagToCheck, AActor* TargetActor);
    static const FGameplayTagContainer& GetOwnedGameplayTag(AActor* TargetActor);

    static void                         GetAbilitySpecsByTag(UAbilitySystemComponent* ASC, const FGameplayTag AssetTags, TArray<FGameplayAbilitySpec*>& Outlist);

private:
    static FGameplayEventData           GetDefaultGameplayEventData(const FGameplayTag EventTag, AActor* Target, AActor* Instigator = nullptr);
};

