// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GBLobbyMatchStartButtonState.h"
#include "GB/Define/Match/GBMatchQueueTypes.h"
#include "GBLobbyMatchStartButtonData.generated.h"


USTRUCT(BlueprintType)
struct FGBLobbyMatchStartButtonVisual
{
    GENERATED_BODY()
    
    // 버튼에 표시할 텍스트
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText ButtonText;

    // 버튼 활성화 여부 
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bEnabled = true;
};

UCLASS(BlueprintType)
class GREEDBOUND_API UGBLobbyMatchStartButtonData : public UDataAsset
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default")
    FGBLobbyMatchStartButtonVisual DefaultVisual;

    // 상태별 표시 매핑
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual", meta = (ForceInlineRow, MapKeyEnum = "/Script/Greedbound.EGBLobbyMatchStartButtonState"))
    TMap<EGBLobbyMatchQueueState, FGBLobbyMatchStartButtonVisual> Visuals;

    // 상태에 해당하는 비주얼 반환(없으면 DefaultVisual)
    UFUNCTION(BlueprintPure, Category = "Visual")
    FGBLobbyMatchStartButtonVisual GetVisual(EGBLobbyMatchQueueState State) const;
};
