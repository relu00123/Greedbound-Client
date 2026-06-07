
#include "GBWeapon.h"
#include "NiagaraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Define/GBCollision.h"
#include "Define/GBSocketName.h"
#include "Define/GBDefine.h"

AGBWeapon::AGBWeapon(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    Mesh->SetCollisionProfileName(GBCOLLISION_PROFILE_NOCOLLISION);
    Mesh->SetupAttachment(RootComponent);

    TrailParticle = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailParticle"));
    TrailParticle->SetupAttachment(RootComponent, GBSOCKETNAME_ATTACKSOCKET01);
    TrailParticle->bAutoActivate = false;
}

const FName& AGBWeapon::GetAttachSocketName() const
{
    return AttachSocketName;
}

void AGBWeapon::SetActiveTrailEffect(const bool bActive)
{
    bTrailEffectActive = bActive;
}

TOptional<FPrimaryAssetId> AGBWeapon::Interact_Implementation(AActor* Other)
{
    GB_NULL_CHECK_WITH_RETURN(Other, {});

    // TODO

    return Super::Interact_Implementation(Other);
}

void AGBWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AGBWeapon, bTrailEffectActive);
}

void AGBWeapon::OnRep_TrailEffectActive()
{
    if (bTrailEffectActive)
    {
        TrailParticle->Activate(false);
    }
    else
    {
        TrailParticle->Deactivate();
    }
}

