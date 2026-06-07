// Fill out your copyright notice in the Description page of Project Settings.

#include "GBPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Define/GBDefine.h"
#include "Define/GBTags.h"
#include "Define/GBCollision.h"
#include "Character/GBCharacterStateTypes.h"
#include "Subsystem/Asset/GBAssetLoadSubsystem.h"
#include "Character/Player/GBPlayerState.h"
#include "Components/GBAbilitySystemComponent.h"
#include "Components/GBCombatComponent.h"
#include "Components/Player/GBEscapeHandlerComponent.h"
#include "Components/Player/GBInventoryComponent.h"
#include "Components/Player/GBInteractionComponent.h"

AGBPlayerCharacter::AGBPlayerCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SpringArm = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
    SpringArm->SetupAttachment(GetRootComponent());
    SpringArm->SetUsingAbsoluteRotation(true);
    SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 75.f));
    SpringArm->SocketOffset = FVector(0.f, 50.f, 0.f);
    SpringArm->TargetArmLength = 300.f;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

    UCharacterMovementComponent* Movement = GetCharacterMovement();
    if (Movement)
    {
        Movement->bUseControllerDesiredRotation = true;
        Movement->RotationRate = FRotator(0.f, 400.f, 0.f);
        Movement->bConstrainToPlane = true;
        Movement->bSnapToPlaneAtStart = true;
    }

    EscapeComponent = CreateDefaultSubobject<UGBEscapeHandlerComponent>(TEXT("EscapeHandler"));
    InteractionComponent = CreateDefaultSubobject<UGBInteractionComponent>(TEXT("Interaction"));

    if (IGBCombatInterface::Execute_GetCombatComponent(this))
    {
        IGBCombatInterface::Execute_GetCombatComponent(this)->SetAttackProfile(GBCOLLISION_PROFILE_PLAYERAOE);
    }
}

void AGBPlayerCharacter::SetCharacterClassType(const EGBCharacterClassType NewClassType)
{
    ClassType = NewClassType;

    LoadCharacterData();
}

void AGBPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocallyControlled())
    {
        GB_BACKEND_LOG(Warning, TEXT("[BeginPlay-Character] It is Locally Controlled : %s"), *GetName());
    }
    else
    {
        GB_BACKEND_LOG(Warning, TEXT("[BeginPlay-Character] It is remotely controlled: %s"), *GetName());
    }
}

void AGBPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AGBPlayerCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    InitializeFromPlayerState();
}

UGBInventoryComponent* AGBPlayerCharacter::GetInventoryComponent_Implementation() const
{
    return CachedInventoryComponent;
}

void AGBPlayerCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    InitializeFromPlayerState();
}

void AGBPlayerCharacter::InitializeFromPlayerState()
{
    AGBPlayerState* GBPS = GetPlayerState<AGBPlayerState>();
    GB_NULL_CHECK(GBPS);

    UGBInventoryComponent* GBInventory = GBPS->GetInventoryComponent();
    GB_NULL_CHECK(GBInventory);

    CachedInventoryComponent = GBInventory;

    if (HasAuthority())
    {
        InteractionComponent->OnInteractedWithItem.AddDynamic(CachedInventoryComponent, &UGBInventoryComponent::OnItemAdded);
    }

    UGBAbilitySystemComponent* GBASC = GBPS->GetAbilitySystemComponent<UGBAbilitySystemComponent>();
    GB_NULL_CHECK(GBASC);

    SetAbilitySystemComponent(GBASC);
    InitAbilitySystemComponent(GBPS);
}

void AGBPlayerCharacter::SprintContinue(const EGBStopState PrevState, const EGBStopState NewState)
{
    if (PrevState == EGBStopState::Stop && NewState != EGBStopState::Stop)
    {
        UGBAbilitySystemComponent* GBASC = GetAbilitySystemComponent<UGBAbilitySystemComponent>();
        if (GBASC && GBASC->HasMatchingGameplayTag(GBTag::Trigger_Common_Sprint))
        {
            GBASC->AbilityTagToggled(GBTag::Trigger_Common_Sprint);
        }
    }
    else if (PrevState != EGBStopState::Stop && NewState == EGBStopState::Stop)
    {
        UGBAbilitySystemComponent* GBASC = GetAbilitySystemComponent<UGBAbilitySystemComponent>();
        if (GBASC)
        {
            const FGameplayTagContainer AssetTags(GBTag::Trigger_Common_Sprint);
            GBASC->CancelAbilities(&AssetTags);
        }
    }
}

void AGBPlayerCharacter::LoadCharacterData()
{
    UGBAssetLoadSubsystem::Get(this)->LoadCharacterDataByClassType(ClassType, GetAbilitySystemComponent<UGBAbilitySystemComponent>());
}
