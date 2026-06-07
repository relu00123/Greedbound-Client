// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GBDedicatedBaseReporter.generated.h"

class UGBDedicatedSocketSubsystem;

UCLASS()
class GREEDBOUND_API UGBDedicatedBaseReporter : public UObject
{
    GENERATED_BODY()
    
    // Dedicated Server용 Reporter Base 클래스
    // WebSocket 메세지 전송 
    // Subsystem 초기화 및 공용 Json  송신함수 포함 

public:
    // GameInstance에서 Subsystem 참조 연결
    virtual void    Init(UGameInstance* GameInstance);

protected:
    // 곹오 JSon 메세지 전송함수
    void            SendJson(const TSharedPtr<FJsonObject>& Json);

protected:
    UPROPERTY()
    TObjectPtr<UGBDedicatedSocketSubsystem> SocketSubsystem; 
};
