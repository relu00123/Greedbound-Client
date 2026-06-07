// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Data/GBInputData.h"
#include "Define/GBDefine.h"
#include "GBInputComponent.generated.h"

UCLASS()
class GREEDBOUND_API UGBInputComponent : public UEnhancedInputComponent
{
    GENERATED_BODY()

public:
    template<typename UserClass, typename TriggeredFuncType, typename ReleasedFuncType, typename ToggledFuncType>
    void SetTaggedInputActionDataAsset(UGBInputData* DataAsset, UserClass* Object, TriggeredFuncType TriggeredFunc, 
        ReleasedFuncType ReleasedFunc, ToggledFuncType ToggledFunc);
};

template<typename UserClass, typename TriggeredFuncType, typename ReleasedFuncType, typename ToggledFuncType>
void UGBInputComponent::SetTaggedInputActionDataAsset(UGBInputData* DataAsset, UserClass* Object, TriggeredFuncType TriggeredFunc, ReleasedFuncType ReleasedFunc, ToggledFuncType ToggledFunc)
{
    GB_NULL_CHECK(DataAsset);
    for (const FGBInputAction& InputAction : DataAsset->AbilityInputActions)
    {
        if (InputAction.TriggerType == EGBTriggerType::Hold)
        {
            if (TriggeredFunc)
            {
                BindAction(InputAction.InputAction, ETriggerEvent::Triggered, Object, TriggeredFunc, InputAction.AssetTag);
            }

            if (ReleasedFunc)
            {
                BindAction(InputAction.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, InputAction.AssetTag);
            }
        }
        else if (InputAction.TriggerType == EGBTriggerType::Toggle)
        {
            if (ToggledFunc)
            {
                BindAction(InputAction.InputAction, ETriggerEvent::Started, Object, ToggledFunc, InputAction.AssetTag);
            }
        }
        else if (InputAction.TriggerType == EGBTriggerType::Trigger)
        {
            if (TriggeredFunc)
            {
                BindAction(InputAction.InputAction, ETriggerEvent::Started, Object, TriggeredFunc, InputAction.AssetTag);
            }
        }
    }
}
