// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GBCharacterBase.h"
#include "Interface/GBInteractionInterface.h"
#include "GBPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UGBEscapeHandlerComponent;
enum class EGBCharacterClassType : uint8;
enum class EGBStopState : uint8;

UCLASS()
class GREEDBOUND_API AGBPlayerCharacter : public AGBCharacterBase, public IGBInteractionInterface
{
    GENERATED_BODY()

public:
    AGBPlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    void                                    SetCharacterClassType(const EGBCharacterClassType NewClassType);
    
private:
    virtual void                            BeginPlay() override;
    virtual void                            Tick(float DeltaTime) override;
    virtual void                            PossessedBy(AController* NewController) override final;

    virtual UGBInventoryComponent*          GetInventoryComponent_Implementation() const override final;
    virtual UGBInteractionComponent*        GetInteractionComponment_Implementation() const override final { return InteractionComponent; }

    void                                    InitializeFromPlayerState();

    void                                    SprintContinue(const EGBStopState PrevState, const EGBStopState NewState);
    virtual void                            LoadCharacterData() override final;

    virtual void                            OnRep_PlayerState() override final;

private:
    UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter")
    TObjectPtr<UCameraComponent>            Camera;

    UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter")
    TObjectPtr<USpringArmComponent>         SpringArm;
        
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Escape", Meta = (AllowPrivateAccess))
    TObjectPtr<UGBEscapeHandlerComponent>   EscapeComponent;

    UPROPERTY(Transient)
    TObjectPtr<UGBInventoryComponent>       CachedInventoryComponent;

    UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter")
    TObjectPtr<UGBInteractionComponent>     InteractionComponent;

    UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter")
    EGBCharacterClassType                   ClassType;
};
