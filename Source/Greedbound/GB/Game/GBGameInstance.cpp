// Fill out your copyright notice in the Description page of Project Settings.

#include "GBGameInstance.h"
#include "Subsystem/Network/Client/GBClientSessionSubsystem.h"
#include "Subsystem/Map/GBMapSelectionSubsystem.h"
#include "Subsystem/UI/GBPopupManagerSubsystem.h"
#include "Subsystem/Map/GBWorldRegistrySubsystem.h"
#include "Subsystem/Network/Client/GBClientSocketRequestHelper.h"
#include "Define/GBDefine.h"

void UGBGameInstance::Init()
{
    Super::Init();

    GB_BACKEND_LOG(Warning, TEXT("GBGameInstance Init Called"));

    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::OnPostLoadMap);

    if (UGBPopupManagerSubsystem* PopupManagerSubsystem = GetSubsystem<UGBPopupManagerSubsystem>())
    {
        PopupManagerSubsystem->SetOutsideClickCatcherClass(OutideClickCatcherWidgetClass);
        PopupManagerSubsystem->SetSharedPopupCanvasWidgetClass(SharedPopupCanvasWidgetClass);
        PopupManagerSubsystem->SetCommonPopupWidgetClass(CommonPopupWidgetClass);
    }

    if (UGBMapSelectionSubsystem* MapSelectionSubsystem = GetSubsystem<UGBMapSelectionSubsystem>())
    {
        GB_BACKEND_LOG(Warning, TEXT("GBGameInstance - SetMapCatalog Called!"));
        MapSelectionSubsystem->SetMapCatalog(MapCatalogData);
    }

    if (UGBWorldRegistrySubsystem* WorldRegisterySubsystem = GetSubsystem<UGBWorldRegistrySubsystem>())
    {
        WorldRegisterySubsystem->SetWorldCatalog(WorldCatalogData);
    }
}

void UGBGameInstance::Shutdown()
{
    //GB_DEV_LOG(TEXT("GBGameInstance ShutDown Called!"));

    if (UGBClientSessionSubsystem* Session = GetSubsystem<UGBClientSessionSubsystem>())
    {
        Session->SendLogoutToServer();
    }

    Super::Shutdown();
}

//void UGBGameInstance::NotifyDungeonLoadedToDS(UWorld* World)
//{
//    if (!World)
//    {
//        GB_DEV_LOG(TEXT("World Does not Exists"));
//        return;
//    }
//
//    // NetMode 확인 : Client에서만 보낼 것임
//    const ENetMode NetMode = World->GetNetMode();
//    if (NetMode == NM_DedicatedServer || NetMode == NM_ListenServer)
//    {
//        // 서버 쪽에서는 준비완료 RPC를 보낼 이유가 없다.
//        GB_DEV_LOG(TEXT("NotifyDungeonLoadedToDS Called On Server, Ignoring.. "));
//        return;
//    }
//
//    if (APlayerController* PC = World->GetFirstPlayerController())
//    {
//        GB_DEV_LOG(TEXT("NotifyDungeonLoadedToDS Called On Client"));
//
//        if (AGBPlayerController* DungeonPC = Cast<AGBPlayerController>(PC))
//        {
//            DungeonPC->Server_NotifyClientLoadedDungeonMap();
//        }
//        
//        else
//        {
//            GB_DEV_LOG(TEXT("Dungeon Player Controller Not found"));
//        }
//    }
//}

void UGBGameInstance::OnPostLoadMap(UWorld* World)
{
    GB_BACKEND_LOG(Warning, TEXT("PostMap Loaded [OnPostLoadMap]"));

    if (UGBWorldRegistrySubsystem* WorldRegisterySubsystem = GetSubsystem<UGBWorldRegistrySubsystem>())
    {
        EGBWorldCategory WorldCategory = WorldRegisterySubsystem->GetCategoryByWorld(World);

        switch (WorldCategory)
        {
        case EGBWorldCategory::Lobby:
        {
            GBClientSocketRequestHelper::SendGamePhaseChangeRequest(FString("Lobby"));
            break;
        }
        case EGBWorldCategory::Dungeon:
        {
            //GB_DEV_LOG(TEXT("OnPostLoadMap Called! case : Dungeon"));
            //NotifyDungeonLoadedToDS(GetWorld());
            break;
        }
        default:
            break;
        }
    }
}
