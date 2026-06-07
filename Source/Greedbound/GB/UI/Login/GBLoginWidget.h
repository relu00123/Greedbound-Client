// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GBLoginWidget.generated.h"

class UButton;
class UEditableText;

UCLASS(BlueprintType)
class GREEDBOUND_API UGBLoginWidget : public UUserWidget
{
    GENERATED_BODY()
    
public:
    virtual void                NativeConstruct() override;

protected:
    UFUNCTION()
    void                        OnLoginClicked();

    UFUNCTION()
    void                        OnRegisterClicked();

    UFUNCTION()
    void                        HandleLoginResult(bool bSuccess);

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton>         Button_Login;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton>         Button_Register;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UEditableText>   ID;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UEditableText>   Password;
};
