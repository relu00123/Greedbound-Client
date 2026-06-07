// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Define/Match/GBMatchInfo.h"
#include "GBDedicatedHttpManagerSubsystem.generated.h"

UCLASS()
class GREEDBOUND_API UGBDedicatedHttpManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:
    void                PostDungeonReady(float DelaySeconds = 0.5f);
    
protected :
    virtual void        Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void        Deinitialize() override;
     
private:
    FDelegateHandle     PostLoadMapWithWorldHandle;
    void                HandlePostLoadMap(UWorld* LoadedWorld);

    static FString      ResolveLocalHostIp();

    void                SendDungeonReadyNow();

    


    void                OnDungeonReadyResponse(bool bOK, int32 Code, TSharedPtr<FJsonObject> Json);

    bool                TryParseMatchInfo(const TSharedPtr<FJsonObject>& Json, FGBMatchInfo& OutMatch) const;

    void                SetMatchInfoToGameModeOnGameThread(const FGBMatchInfo& AssignedMatchInfo);

    void                PostJson(const FString& Url, const TSharedRef<class FJsonObject>& Json, const TMap<FString, FString>& Headers, TFunction<void(bool bOK, int32 Code, TSharedPtr<FJsonObject>)> Completion = nullptr);
};
