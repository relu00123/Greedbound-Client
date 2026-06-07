// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GBRightClickMenuTypes.h"
#include "Define/User/UserInfo.h"
#include "Define/UI/GBLobbyUITypes.h"
 
class GREEDBOUND_API GBRightClickMenuBuilder
{
public:
    static TArray<FRightClickMenuEntry> BuildForUser(const FUserInfo& TargetUser, const UGameInstance* GameInstance, EGBLobbyPlayerListType PlayerListType, bool isEmptySlot);
};
