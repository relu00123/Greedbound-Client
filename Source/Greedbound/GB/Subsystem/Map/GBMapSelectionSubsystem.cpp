// Fill out your copyright notice in the Description page of Project Settings.


#include "GB/Subsystem/Map/GBMapSelectionSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GB/Define/GBDefine.h"

void UGBMapSelectionSubsystem::SetMapCatalog(TSoftObjectPtr<UGBGameMapCatalogData> InCatalog)
{
    // const EGBMapType Old = CurrentMapType;
    MapCatalogRef = InCatalog;

    GB_BACKEND_LOG(Warning, TEXT("SetMapCatalog Called!"));

    const UGBGameMapCatalogData* Catalog = MapCatalogRef.LoadSynchronous();

    if (InCatalog)
    {
        AvailableMaps = Catalog ? Catalog->Maps : TMap<EGBGameMapType, FGBGameMapInfo>{};
    }

    else
    {
        GB_BACKEND_LOG(Warning, TEXT("SetMapCatalog In Catalog Failed!"));
    }

    // default맵 설정됨에따라 UI반영위한 Delegate만들 수도 있을 것.. 
}

void UGBMapSelectionSubsystem::LogAvailableMaps() const
{
    GB_BACKEND_LOG(Warning, TEXT("==== [AvailableMaps Dump] (%d entries) ===="), AvailableMaps.Num());

    for (const TPair<EGBGameMapType, FGBGameMapInfo>& KV : AvailableMaps)
    {
        const EGBGameMapType MapType = KV.Key;
        const FGBGameMapInfo& Info = KV.Value;

        // Key (Enum) 문자열화
        const UEnum* EnumObj = StaticEnum<EGBGameMapType>();
        FString MapTypeName = EnumObj ? EnumObj->GetNameStringByValue((int64)MapType) : TEXT("<?>");

        // FGBGameMapInfo 상세
        const int32     NumericId = Info.MapNumericId;
        const FName     MapId = Info.MapId;
        const FString   DisplayNameStr = Info.DisplayName.ToString();

        const FString   MapAssetStr = Info.MapAsset.IsNull()
            ? TEXT("None")
            : Info.MapAsset.ToString(); // SoftObjectPtr<UWorld> 경로

        const FString   ThumbStr = Info.MapThumbnail.IsNull()
            ? TEXT("None")
            : Info.MapThumbnail.ToString(); // SoftObjectPtr<UTexture2D> 경로

        UE_LOG(LogTemp, Warning,
            TEXT("MapType=%s  (enum=%d)\n  NumericId=%d\n  MapId=%s\n  DisplayName=%s\n  MapAsset=%s\n  MapThumbnail=%s"),
            *MapTypeName,
            (int32)MapType,
            NumericId,
            *MapId.ToString(),
            *DisplayNameStr,
            *MapAssetStr,
            *ThumbStr
        );
    }

    const UEnum* EnumPtr = StaticEnum<EGBGameMapType>();
    if (EnumPtr)
    {
        FString EnumName = EnumPtr->GetNameStringByValue((int64)SelectedMapType);
        UE_LOG(LogTemp, Warning, TEXT("SelectedMap : %s"), *EnumName);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SelectedMap : <Enum not found>"));
    }

    UE_LOG(LogTemp, Warning, TEXT("==========================================="));
}

void UGBMapSelectionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    SelectedMapType = EGBGameMapType::NetTestDungeon;
}

//EGBGameMapType UGBMapSelectionSubsystem::FindMapTypeByWorld(const UWorld* World) const
//{
//    if (!World) return EGBGameMapType::None;
//
//    // 패키지 경로: "/Game/Maps/Lobby"
//    const FString LoadedPkg = World->GetOutermost()->GetName();
//
//    // 짧은 이름: "Lobby" (PIE 접두어 제거 true)
//    const FString LoadedShort = UGameplayStatics::GetCurrentLevelName(const_cast<UWorld*>(World), /*bRemovePrefix=*/true);
//
//    for (const auto& KVP : AvailableMaps)
//    {
//        const FSoftObjectPath P = KVP.Value.MapAsset.ToSoftObjectPath(); // "/Game/Maps/Lobby.Lobby"
//
//        if (LoadedPkg.Equals(P.GetLongPackageName(), ESearchCase::IgnoreCase))
//            return KVP.Key;
//
//        if (LoadedShort.Equals(P.GetAssetName(), ESearchCase::IgnoreCase))
//            return KVP.Key;
//    }
//    return EGBGameMapType::None;
//}
