// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Define/Character/GBClassType.h"
#include "GBLobbyClassSelectPortraitTile.generated.h"


class UGBClassSelectData;
class UButton;
class UBorder;
class UImage;

UCLASS()
class GREEDBOUND_API UGBLobbyClassSelectPortraitTile : public UUserWidget
{
    GENERATED_BODY()
    
public:
    // 외부에서 호출하는 안전한 바인딩 API (BP/C++ 모두 사용 가능)
    UFUNCTION(BlueprintCallable, Category = "Event")
    void                                BindOnTileClicked(UObject* Target, FName FunctionName);

    UFUNCTION(BlueprintCallable, Category = "Event")
    void                                UnbindOnTileClicked(UObject* Target, FName FunctionName);

 
    // 선택 상태를 코드/Blueprint에서 변경
    UFUNCTION(BlueprintCallable)
    void                                SetSelected(bool bInSelected);

    UFUNCTION(BlueprintCallable)
    void                                SetIsCurrent(bool bInCurrent) { SetSelected(bInCurrent); }

    UFUNCTION(BlueprintCallable)
    void                                SetTempSelected(bool bInTempSelected);    // 임시 선택(노란색)

    EGBCharacterClassType                GetClassType() { return ClassType; }

protected:
    virtual void                        NativeConstruct() override;
    virtual void                        NativePreConstruct() override;
    virtual void                        NativeDestruct() override;

private:
    UFUNCTION()
    void                                HandleClicked();
    void                                ApplyVisual();

private:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTileClicked, EGBCharacterClassType, ClassType);


    UPROPERTY(BlueprintAssignable, Category = "Event")
    FOnTileClicked                        OnTileClicked;

    // UMG 바인딩 
    UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidget, AllowPrivateAccess = "true"))
    TObjectPtr<UButton>                    TileButton;

    UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidget, AllowPrivateAccess = "true"))
    TObjectPtr<UBorder>                    FrameBorder;

    UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidget, AllowPrivateAccess = "true"))
    TObjectPtr<UImage>                    PortraitImage;

    // 데이터
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
    EGBCharacterClassType                ClassType = EGBCharacterClassType::Knight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UGBClassSelectData>        PortraitData;

    // 상태
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
    bool                                bSelected = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
    bool                                bTempSelected = false;
};
