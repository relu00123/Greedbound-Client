// Fill out your copyright notice in the Description page of Project Settings.

#include "GB/Character/Lobby/GBLobbyPlayerController.h"

void AGBLobbyPlayerController::BeginPlay()
{
    Super::BeginPlay();

    bShowMouseCursor = true;

    FInputModeUIOnly InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);
}
