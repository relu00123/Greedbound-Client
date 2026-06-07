#pragma once

UENUM(BlueprintType)
enum class EGBStanceState : uint8
{
    Stand,
    Crouch,
};

UENUM(BlueprintType)
enum class EGBStopState : uint8
{
    Stop,
    Move,
};

UENUM(BlueprintType)
enum class EGBMoveState : uint8
{
    Walk,
    Sprint,
};

UENUM(BlueprintType)
enum class EGBCombatState : uint8
{
    Idle,
    Attack,
    Knockback,
    Dead,
};

UENUM(BlueprintType)
enum class EGBDirection : uint8 
{
    NoReact,
    Forward,
    Left,
    Right,
    Backward
};
