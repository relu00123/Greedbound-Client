// Fill out your copyright notice in the Description page of Project Settings.

#include "Utils/GBBlueprintFunctionLibrary.h"
#include "Define/GBAnimSectionName.h"

FName UGBBlueprintFunctionLibrary::GetAttackSectionNameByComboIndex(const uint8 ComboIndex)
{
    return FGBAnimSectionNameHelper::GetAttackSectionNameByComboIndex(ComboIndex);
}

FName UGBBlueprintFunctionLibrary::GetDeathSectionNameByDirection(const float Direction)
{
    return FGBAnimSectionNameHelper::GetDeathSectionNameByDirection(Direction);
}

FName UGBBlueprintFunctionLibrary::GetHitReactionNameByDirection(const float Direction)
{
    return FGBAnimSectionNameHelper::GetHitReactionNameByDirection(Direction);
}
