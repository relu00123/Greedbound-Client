// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/Popup/RightClick/GBRightClickMenuTypes.h"
#include "GBLobbyHUD.generated.h"

class UGBLobbyWidget;
class UGBRightClickMenuWidget;
//class UGBRightClickMenuWrapperWidget;

UCLASS()
class GREEDBOUND_API AGBLobbyHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void                BeginPlay() override;

    FORCEINLINE UGBLobbyWidget* GetLobbyMainWidget() const { return LobbyWidget; }

protected:
    // 생성할 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UGBLobbyWidget> LobbyWidgetClass;

    // 실제 인스턴스
    UPROPERTY()
    TObjectPtr<UGBLobbyWidget>  LobbyWidget;
};
