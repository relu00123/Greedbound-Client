// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Define/GamePhase/GBClientGamePhase.h"


class UGBDedicatedSocketSubsystem;
class UGameInstance;

 
class FJsonObject; 
/**
 * 
 */
class GREEDBOUND_API UGBDedicatedSocketRequestHelper  
{
public :
    // 특정 Client의 GamePhase를 InDungeon으로 변경 요청
    static void SendChangeClientGamePhase(const FString& ChangedClientToken, EGBClientGamePhase GamePhase);
	
	
private:
    static void SendJsonRequest(const FString& Type, const TSharedPtr<FJsonObject>& Payload = nullptr);
	
	
	
};
