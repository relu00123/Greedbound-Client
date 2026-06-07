#pragma once

UENUM(BlueprintType)
enum class EGBItemType : uint8
{
    NONE,
    Misc,
    Consumable,
    Equipment,
};

UENUM(BlueprintType)
enum class EGBItemGrade : uint8
{
    Common,
    UnCommon,
    Rare,
    Epic,
    Legandary,
};

UENUM(BlueprintType)
enum class EGBEquipSlotType : uint8
{
    Head,
    Chest,
    Legs,
    Weapon,
};

UENUM(BlueprintType)
enum class EGBInteractionType : uint8
{
    None,
    Item,
    Activate,
};
