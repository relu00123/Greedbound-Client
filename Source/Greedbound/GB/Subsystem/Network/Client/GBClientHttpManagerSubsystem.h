// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GBClientHttpManagerSubsystem.generated.h"

DECLARE_DELEGATE_FourParams(FOnLoginResult, bool /*bSuccess*/, const FString& /*Message*/, const FString& /*Token*/, FString& /*NickName*/);

class IHttpRequest;
class IHttpResponse;

using FHttpRequestPtr = TSharedPtr<IHttpRequest, ESPMode::ThreadSafe>;
using FHttpResponsePtr = TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>;

UCLASS()
class GREEDBOUND_API UGBClientHttpManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:
    // Login
    void    SendLoginRequest(const FString& ID, const FString& PWD, FOnLoginResult OnResult);
    void    SendLogoutRequest(const FString& Token);

    // Register
    void    RegisterUser(const FString& ID, const FString& PWD, TFunction<void(bool, const FString&)> Callback);

    // Find Match
    void    SendStartMatch(const FString& Token, int32 MapNumericId,  TFunction<void(bool, const FString&)> Callback, bool bAllowOhters = false);

private:
    void    OnLoginResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FOnLoginResult OnResult);

private:
    FString ServerTravelURL = TEXT("http://localhost:3000/login");
    
};
