
#include "GB/Subsystem/Network/Dedicated/GBDedicatedSocketResponseRegistrar.h"
#include "Subsystem/Network/Dedicated/GBDedicatedSocketSubsystem.h"


void GBDedicatedSocketResponseRegistrar::RegisterAll(UGBDedicatedSocketSubsystem* Socket, UGameInstance* GameInstance)
{
    if (!Socket) return;

    /*RegisterLobbyHandlers(Socket, GameInstance);
    RegisterFriendHandlers(Socket, GameInstance);
    RegisterMatchHandlers(Socket, GameInstance);
    RegisterPartyHandlers(Socket, GameInstance);
    RegisterGamePhaseHandlers(Socket, GameInstance);*/
}
