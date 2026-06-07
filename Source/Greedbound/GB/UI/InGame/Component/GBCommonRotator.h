// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonRotator.h"
#include "GBCommonRotator.generated.h"

/**
 * 설정에서 세부 옵션 변경할 때 쓰이는 버튼 ex) <옵션1> 에서 '<', '>'
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class GREEDBOUND_API UGBCommonRotator : public UCommonRotator
{
	GENERATED_BODY()
	
public:
    void SetSelectedOptionByText(const FText& InOptionText);
};
