#pragma once

UENUM(BlueprintType)
enum class EGBCharacterClassType : uint8
{
    None UMETA(Hidden),
    Knight,
    Cleric,
    Archer,
    Mage, 
    Max UMETA(Hidden)
};
