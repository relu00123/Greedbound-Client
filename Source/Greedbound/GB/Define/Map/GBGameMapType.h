#pragma once 
#include "CoreMinimal.h"
#include "GBGameMapType.generated.h"
 

class UWorld;
class UTexture2D;

UENUM(BlueprintType)
enum class EGBGameMapType : uint8
{
    None                UMETA(DisplayName = "None"),
    GoblinCave          UMETA(DisplayName = "Goblin Cave"),
    ForgottenCastle     UMETA(DisplayName = "Forgotten Castle"),
    NetTestDungeon      UMETA(DisplayName = "NetTest Dungeon"),
    MAX 
};

//UENUM(BlueprintType)
//enum class EGBMapCategory : uint8
//{
//    None,
//    Lobby,
//    Dungeon
//};

USTRUCT(BlueprintType)
struct FGBGameMapInfo
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32                       MapNumericId = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName                       MapId;            // 내부/로그용 키

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText                       DisplayName;    // 해당 맵에 대한 표시이름. 클라는 UI로 인해 필수 

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UWorld>      MapAsset;        // Travel할 Map Asset 저장

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UTexture2D>  MapThumbnail;    // 로비에서 보여줄 맵에대한 Thumbnail UI 
};

 
