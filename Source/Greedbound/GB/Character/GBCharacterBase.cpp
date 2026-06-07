// Fill out your copyright notice in the Description page of Project Settings.

#include "GBCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/GBAbilitySystemComponent.h"
#include "Components/GBMovementStateComponent.h"
#include "Components/GBCombatComponent.h"
#include "Define/GBDefine.h"
#include "Define/GBCollision.h"
#include "Subsystem/Asset/GBAssetLoadSubsystem.h"
#include "AbilitySystem/Attribute/GBSpeedAttributeSet.h"

AGBCharacterBase::AGBCharacterBase(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bReplicates = true;
    SetReplicatingMovement(true);

    CombatComponent = CreateDefaultSubobject<UGBCombatComponent>(TEXT("CombatComponent"));
    MovementStateComponent = CreateDefaultSubobject<UGBMovementStateComponent>(TEXT("MovementStateComponent"));
}

void AGBCharacterBase::BeginPlay()
{
    Super::BeginPlay();
}

void AGBCharacterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AGBCharacterBase::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickMontagesAndRefreshBonesWhenPlayingMontages;
}

UAbilitySystemComponent* AGBCharacterBase::GetAbilitySystemComponent() const
{
    return Cast<UAbilitySystemComponent>(AbilitySystemComponent);
}

void AGBCharacterBase::SetAbilitySystemComponent(UGBAbilitySystemComponent* ASC)
{
    GB_NULL_CHECK(ASC);
    AbilitySystemComponent = ASC;

    GB_NULL_CHECK(MovementStateComponent);
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UGBSpeedAttributeSet::GetCurrentSpeedAttribute()).AddUObject(MovementStateComponent, &UGBMovementStateComponent::OnCurrentSpeedChanged);
}

void AGBCharacterBase::Multicast_BeRagdoll_Implementation(bool bActive)
{
    if (bActive)
    {
        GetMesh()->SetSimulatePhysics(true);
        GetMesh()->SetCollisionProfileName(GBCOLLISION_PROFILE_RAGDOLL);
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    else
    {
        GetMesh()->SetSimulatePhysics(false);
        GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
}

void AGBCharacterBase::InitAbilitySystemComponent(AActor* OwnerActor)
{
    GB_VALID_CHECK(OwnerActor);
    GB_NULL_CHECK(AbilitySystemComponent);

    if (HasAuthority())
    {
        LoadCharacterData();
    }

    AbilitySystemComponent->InitAbilityActorInfo(OwnerActor, this);
}

void AGBCharacterBase::SetRagdoll_Implementation(bool bRagdoll)
{
    if (HasAuthority())
    {
        Multicast_BeRagdoll(bRagdoll);
    }
}

void AGBCharacterBase::LoadCharacterData()
{
    GB_CONDITION_CHECK(CharacterDataAssetId.IsValid());

    UGBAssetLoadSubsystem::Get(this)->LoadCharacterDataByAssetId(CharacterDataAssetId, AbilitySystemComponent);
}
