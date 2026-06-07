#include "InteractionObject/GBInteractableBase.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Define/GBItemTypes.h"
#include "Define/GBDefine.h"

AGBInteractableBase::AGBInteractableBase(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
    :Super(ObjectInitializer)
{
    Sphere = CreateDefaultSubobject<USphereComponent>("CollisionSphere");
    SetRootComponent(Sphere);

    InteractionWidget = CreateDefaultSubobject<UWidgetComponent>("InteractionWidget");
    InteractionWidget->SetupAttachment(Sphere);
    InteractionWidget->SetActive(false);
}

EGBInteractionType AGBInteractableBase::GetInteractionType_Implementation() const
{
    GB_LOG(TEXT("Implementation Missing!"));
    return EGBInteractionType::None;
}

TOptional<FPrimaryAssetId> AGBInteractableBase::Interact_Implementation(AActor* Other)
{
    Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (HasAuthority())
    {
        Destroy();
    }
    else
    {
        IGBInteractable::Execute_LookAway(this);
    }

    return {};
}

void AGBInteractableBase::LookAt_Implementation()
{
    if (HasAuthority())
    {
        return;
    }

    InteractionWidget->SetVisibility(true);
}

void AGBInteractableBase::LookAway_Implementation()
{
    if (HasAuthority())
    {
        return;
    }

    InteractionWidget->SetVisibility(false);
}
