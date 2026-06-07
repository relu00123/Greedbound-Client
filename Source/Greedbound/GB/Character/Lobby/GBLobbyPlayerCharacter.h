// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GBLobbyPlayerCharacter.generated.h"

UCLASS()
class GREEDBOUND_API AGBLobbyPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AGBLobbyPlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
                 
protected:         
    virtual void BeginPlay() override;
};
