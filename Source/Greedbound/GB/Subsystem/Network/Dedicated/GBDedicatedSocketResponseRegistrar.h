#pragma once

#include "CoreMinimal.h"

class UGBDedicatedSocketSubsystem;
class UGameInstance;

// 웹 소켓 Response 메세지 헨들러 등록 전용 클래스 
class GREEDBOUND_API GBDedicatedSocketResponseRegistrar
{
public:
    // 메시지 핸들러 전체 등록
    static void RegisterAll(UGBDedicatedSocketSubsystem* Socket, UGameInstance* GameInstance);

private:
    // 클라에서는 아래 처럼 카테고리 나눠서 작업했는데 이런 느낌으로 작업하면 될 듯 
    // 하위 섹션별 등록 함수들
    /*static void RegisterLobbyHandlers(UGBClientSocketSubsystem* Socket, UGameInstance* GameInstance);
    static void RegisterFriendHandlers(UGBClientSocketSubsystem* Socket, UGameInstance* GameInstance);
    static void RegisterMatchHandlers(UGBClientSocketSubsystem* Socket, UGameInstance* GameInstance);
    static void RegisterPartyHandlers(UGBClientSocketSubsystem* Socket, UGameInstance* GameInstance);
    static void RegisterGamePhaseHandlers(UGBClientSocketSubsystem* Socket, UGameInstance* GameInstance);*/
    // 필요 시 확장...
};
