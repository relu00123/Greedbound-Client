// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GBGameInstance.generated.h"

class UGBOutsideClickCatcherWidget;
class UGBSharedPopupCanvasWidget;
class UGBCommonPopupWidget;
class UGBWorldCatalogData;
class UGBGameMapCatalogData;

UCLASS()
class GREEDBOUND_API UGBGameInstance : public UGameInstance
{
    GENERATED_BODY()
    
public:
    UFUNCTION()
    void                                        OnPostLoadMap(UWorld* World);

protected:
    virtual void                                Init() override;
    virtual void                                Shutdown() override;

private :
    //void                                        NotifyDungeonLoadedToDS(UWorld* World);

public:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UGBOutsideClickCatcherWidget>   OutideClickCatcherWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UGBSharedPopupCanvasWidget>     SharedPopupCanvasWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UGBCommonPopupWidget>           CommonPopupWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Maps")
    TSoftObjectPtr<UGBGameMapCatalogData>       MapCatalogData;

    UPROPERTY(EditDefaultsOnly, Category = "Maps")
    TSoftObjectPtr<UGBWorldCatalogData>         WorldCatalogData;
};
