// Fill out your copyright notice in the Description page of Project Settings.

#include "GBLobbyPlayerCharacter.h"

AGBLobbyPlayerCharacter::AGBLobbyPlayerCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void AGBLobbyPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AGBLobbyPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AGBLobbyPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

