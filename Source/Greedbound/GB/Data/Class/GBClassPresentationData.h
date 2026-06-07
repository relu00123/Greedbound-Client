// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GBClassPresentationData.generated.h"


USTRUCT(BlueprintType)
struct FGBClassPresentation
{
    GENERATED_BODY()

    // 클래스의 기본 Pawn(클래스별로 다른 Pawn을 쓰는 경우)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<APawn> PawnClass;

    // 클래스 전용 HUD (없으면 null 허용)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<AHUD> HUDClass = nullptr;

    // 필요하면 여기 확장: 레티클 위젯, Portrait, UI 스킨 등
    // UPROPERTY(EditDefaultsOnly) TSoftClassPtr<UUserWidget> MainWidget;
};

UCLASS()
class GREEDBOUND_API UGBClassPresentationData : public UDataAsset
{
    GENERATED_BODY()
    
public:
    // EGBCharacterClassType :  표현 데이터
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Class")
    TMap<EGBCharacterClassType, FGBClassPresentation> Entries;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool Find(EGBCharacterClassType ClassType, FGBClassPresentation& Out) const
    {
        if (const FGBClassPresentation* Found = Entries.Find(ClassType))
        {
            Out = *Found;
            return true;
        }

        return false;
    }
};
