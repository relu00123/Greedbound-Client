#pragma once

#include "GBUIStructTypes.generated.h"

USTRUCT()
struct FOptionsDataEditConditionDescriptor
{
    GENERATED_BODY()

public:
    void SetEditConditionFunc(TFunction<bool()> InEditConditionFunc)
    {
        EditConditionFunc = InEditConditionFunc;       
    }

    bool IsValid() const 
    {
        return EditConditionFunc != nullptr;
    }

    bool IsEditConditionMet() const 
    {
        if (IsValid())
        {
            return EditConditionFunc();
        }

        return true;
    }

    FString GetDisableRichReason() const
    {
        return DisabledRichReason;
    }

    void SetDisabledRichReason(const FString& InRichReason)
    {
        DisabledRichReason = InRichReason;
    }

    bool HasForcedStringValue() const
    {
        return DisableForcedStringValue.IsSet();
    }

    FString GetDisableForcedStringValue() const 
    {
        return DisableForcedStringValue.GetValue();
    }

    void SetDisableForcedStringValue(const FString& InForcedValue)
    {
        DisableForcedStringValue = InForcedValue;
    }

private:
    TFunction<bool()>   EditConditionFunc;
    FString             DisabledRichReason;
    TOptional<FString>  DisableForcedStringValue;
};
