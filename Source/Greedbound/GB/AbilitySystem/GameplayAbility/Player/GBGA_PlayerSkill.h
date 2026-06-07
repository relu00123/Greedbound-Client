// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GB/AbilitySystem/GameplayAbility/GBGA_Base.h"
#include "GBGA_PlayerSkill.generated.h"

class UTexture2D;
/**
 Skill Icon 정보를 가지고 있어야 하는 Player Skill Ability의 Base Class
 */
UCLASS()
class GREEDBOUND_API UGBGA_PlayerSkill : public UGBGA_Base
{
	GENERATED_BODY()

public:
    UTexture2D*                 GetSkillIcon() const { return SkillIcon.LoadSynchronous(); }
	
private:
    UPROPERTY(EditDefaultsOnly, Category = "SklillAbilityInfo")
    TSoftObjectPtr<UTexture2D>  SkillIcon;
};
