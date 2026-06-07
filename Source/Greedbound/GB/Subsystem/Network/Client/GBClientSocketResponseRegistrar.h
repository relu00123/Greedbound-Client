// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class UGBClientSocketSubsystem;
class UGameInstance;

// 웹 소켓 Response 메세지 헨들러 등록 전용 클래스 
class GREEDBOUND_API GBClientSocketResponseRegistrar
{
public:
    // 메시지 핸들러 전체 등록
    static void RegisterAll(UGBClientSocketSubsystem* Socket, UGameInstance* GameInstance);

private:
    // 하위 섹션별 등록 함수들
    static void RegisterLobbyHandlers(UGBClientSocketSubsystem* Socket, UGameInstance* GameInstance);
    static void RegisterFriendHandlers(UGBClientSocketSubsystem* Socket, UGameInstance* GameInstance);
    static void RegisterMatchHandlers(UGBClientSocketSubsystem* Socket, UGameInstance* GameInstance);
    static void RegisterPartyHandlers(UGBClientSocketSubsystem* Socket, UGameInstance* GameInstance);
    static void RegisterGamePhaseHandlers(UGBClientSocketSubsystem* Socket, UGameInstance* GameInstance);
    // 필요 시 확장...
};
