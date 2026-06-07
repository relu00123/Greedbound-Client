// Fill out your copyright notice in the Description page of Project Settings.

#include "GBEscapeHandlerComponent.h"
#include "GB/Subsystem/Network/Client/GBClientHttpManagerSubsystem.h"
#include "GB/Subsystem/Network/Dedicated/GBDedicatedSocketSubsystem.h"
#include "GB/Subsystem/Network/Dedicated/GBDedicatedSessionSubsystem.h"
#include "Character/Player/GBPlayerState.h"
#include "GameFramework/Character.h"
#include "Utils/GBJsonUtils.h"
#include "Network/Payload/FEscapePayload.h"

// Sets default values for this component's properties
UGBEscapeHandlerComponent::UGBEscapeHandlerComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    // ...
}

void UGBEscapeHandlerComponent::Escape()
{
    UE_LOG(LogTemp, Warning, TEXT("[Escape] Need to Work on Escape Logic"));

    // 7월 7일에 잘 작동하는지 임시로 확인해본 코드 
    //AActor* Owner = GetOwner();
    //if (!Owner || !Owner->HasAuthority()) return;

    //APlayerController* Controller = Cast<APlayerController>(Owner->GetInstigatorController());
    //if (!Controller) return;

    //// 클라이언트를 로비 맵으로 이동 (LevelTravel)
    //const FString LobbyMapPath = TEXT("/Game/GB/Maps/Login"); // 실제 로비 맵 경로
    //Controller->ClientTravel(LobbyMapPath, ETravelType::TRAVEL_Absolute);
    // 7월 7일에 작동한 코드 완료

    if (!GetOwner() || !GetOwner()->HasAuthority())
        return;
     

    ACharacter* Character = Cast<ACharacter>(GetOwner()); 
    if (!Character) return;

    AGBPlayerState* PS = Character->GetPlayerState<AGBPlayerState>();
    if (!PS)
    {
        UE_LOG(LogTemp, Error, TEXT("Escape failed! : No PlayerState"));
        return;
    }

    FString PlayerToken = PS->GetAuthToken();
    FString CharacterID = PS->GetCharacterId();


    // 메시지 준비
    FEscapePayload Payload;
    Payload.DungeonToken = GetWorld()->GetGameInstance()->GetSubsystem<UGBDedicatedSessionSubsystem>()->GetDungeonToken();
    Payload.PlayerToken = PlayerToken;


    //UE_LOG(LogTemp, Warning, TEXT("[Escape] DungeonToken : %s"), *(Payload.DungeonToken));
    //UE_LOG(LogTemp, Warning, TEXT("[Escape] PlayerToken  : $s"), *(Payload.PlayerToken));

    // 전송
    UGBDedicatedSocketSubsystem* SocketSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UGBDedicatedSocketSubsystem>();
    if (SocketSubsystem)
    {
        SocketSubsystem->SendEscapeRequest(Payload);
    }
}


 
