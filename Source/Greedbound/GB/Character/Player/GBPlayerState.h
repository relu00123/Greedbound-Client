// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "Define/Character/GBClassType.h"
#include "Interface/GBInteractionInterface.h"
#include "GBPlayerState.generated.h"

class UAbilitySystemComponent;
class UGBInventoryComponent;

UCLASS()
class GREEDBOUND_API AGBPlayerState : public APlayerState, public IAbilitySystemInterface
{
    GENERATED_BODY()
    
public:
    AGBPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // Server
    FORCEINLINE const FString&          GetAuthToken() const { return AuthToken; }
    FORCEINLINE const FString&          GetCharacterId() const { return CharacterId; }

    FORCEINLINE void                    SetAuthToken(const FString& InToken) { AuthToken = InToken; }
    FORCEINLINE void                    SetCharacterId(const FString& InCharacterId) { CharacterId = InCharacterId; }
    
    // AbilitySystem
    virtual UAbilitySystemComponent*    GetAbilitySystemComponent() const override final { return AbilitySystemComponent; }

    template <typename T>
    T*                                  GetAbilitySystemComponent() const { return Cast<T>(GetAbilitySystemComponent()); }

    UGBInventoryComponent*              GetInventoryComponent() const { return InventoryComponent; }

    void                                SetSelectedClass(EGBCharacterClassType InClass);
    FORCEINLINE EGBCharacterClassType   GetSelectedClass() const { return SelectedClass;  }

    FORCEINLINE void                    SetAssignedInGameHUDClass(TSubclassOf<AHUD> InHUDClass) { AssignedInGameHUDClass = InHUDClass; }
    TSubclassOf<AHUD>                   GetAssignedInGameHUDClass() const { return AssignedInGameHUDClass; }

    /** 팀 ID 조회 (GameMode, HUD 등에서 참조) */
    FORCEINLINE const FString&          GetTeamId() const { return TeamId; }
    FORCEINLINE void                    SetTeamId(const FString& InTeamId) { TeamId = InTeamId; }

protected:
    virtual void                        GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    UPROPERTY(Transient)
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    FString                             AuthToken;
    FString                             CharacterId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team", meta = (AllowPrivateAccess = "true"))
    FString                             TeamId;

    UPROPERTY(Replicated)
    EGBCharacterClassType               SelectedClass = EGBCharacterClassType::None;

    UPROPERTY(Replicated)
    TSubclassOf<AHUD>                   AssignedInGameHUDClass;

    UPROPERTY(Replicated)
    TObjectPtr<UGBInventoryComponent>   InventoryComponent;
};
