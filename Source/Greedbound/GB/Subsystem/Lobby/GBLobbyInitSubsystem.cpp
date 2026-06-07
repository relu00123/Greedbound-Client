// Fill out your copyright notice in the Description page of Project Settings.


#include "GB/Subsystem/Lobby/GBLobbyInitSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UGBLobbyInitSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    RemainingSteps = ELobbyInitStep::ALL;
}

void UGBLobbyInitSubsystem::SetTargetInitSteps(ELobbyInitStep TargetSteps)
{
    RemainingSteps = TargetSteps;
}

void UGBLobbyInitSubsystem::MarkStepCompleted(ELobbyInitStep Step)
{
    if (EnumHasAllFlags(RemainingSteps, Step))
    {
        RemainingSteps &= ~Step;

        UE_LOG(LogTemp, Log, TEXT("Lobby Init Step '%s' completed."), *StaticEnum<ELobbyInitStep>()->GetNameStringByValue((int64)Step));

        if (RemainingSteps == ELobbyInitStep::None)
        {
            UE_LOG(LogTemp, Log, TEXT("All Lobby Initialization Steps Completed!"));
            HandleAllInitCompleted();       // 기본적으로 해야하는 기능은 맵이동. 
            OnLobbyInitialized.Broadcast(); // 외부에서 Binding을 해 두었다면 그들에도 알려준다.
        }
    }
}

void UGBLobbyInitSubsystem::HandleAllInitCompleted()
{
    // 유저매니저의 초기화가 완료된 시점이다.
    // PartyManager의 PlayerInfo중 나 자신에 대해 초기화를 진행한다. 


    if (IsInitializationComplete())
    {
        UE_LOG(LogTemp, Log, TEXT("[UGBLobbyInitSubsystem] 모든 초기화 완료! 로비 맵으로 이동합니다."));

        const FName LobbyMapName = TEXT("/Game/GB/Maps/Lobby");
        UGameplayStatics::OpenLevel(GetWorld(), LobbyMapName);

        // GamePhase를 로비로 변경하라고 서버에 요청 보냄 근데... 이건 로비 래벨이 다열린 후에.. 
        // GameInstance의 OnPostLoadMap에서 해주고 있음 

    }
}
