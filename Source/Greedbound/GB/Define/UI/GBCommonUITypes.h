#pragma once

UENUM(BlueprintType)
enum class EGBConfirmScreenType : uint8
{
    Ok,
    YesNo,
    OkCancel,
    None UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EGBConfirmScreenButtonType : uint8
{
    Confirmed,
    Cancelled,
    Closed,
    None UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EGBOptionsListDataModifyReason : uint8
{
    DirectlyModified,
    DependencyModified,
    ResetToDefault,
};
