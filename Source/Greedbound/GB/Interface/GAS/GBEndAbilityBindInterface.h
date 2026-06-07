#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GBEndAbilityBindInterface.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAbilityEndedDelegate);

UINTERFACE(MinimalAPI)
class UGBEndAbilityBindInterface : public UInterface
{
    GENERATED_BODY()
};

class GREEDBOUND_API IGBEndAbilityBindInterface
{
    GENERATED_BODY()

public:
    virtual FOnAbilityEndedDelegate* GetOnAbilityEndedDelegate() = 0;
};
