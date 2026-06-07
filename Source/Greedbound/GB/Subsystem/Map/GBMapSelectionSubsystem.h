// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Define/Map/GBGameMapType.h"
#include "Data/Map/GBGameMapCatalogData.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GBMapSelectionSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class GREEDBOUND_API UGBMapSelectionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
    
public:
    UFUNCTION(BlueprintCallable, Category = "Maps")
    void                                        SetMapCatalog(TSoftObjectPtr<UGBGameMapCatalogData> InCatalog);

    FORCEINLINE EGBGameMapType                  GetSelectedMapType() { return SelectedMapType; }
    FORCEINLINE const FGBGameMapInfo*           GetSelectedMapInfo() { LogAvailableMaps(); return AvailableMaps.Find(SelectedMapType); }

    void                                        LogAvailableMaps() const;
    

    /*UFUNCTION(BlueprintCallable, Category = "Maps")
    EGBMapType                                FindMapTypeByWorld(const UWorld* World) const;*/

protected:
    virtual void                                Initialize(FSubsystemCollectionBase& Collection) override;
    
    
private:
    UPROPERTY(EditAnywhere, Category="Maps")
    TSoftObjectPtr<UGBGameMapCatalogData>       MapCatalogRef;

    UPROPERTY(VisibleAnywhere, Category="Maps")
    TMap<EGBGameMapType, FGBGameMapInfo>        AvailableMaps; 

    EGBGameMapType                              SelectedMapType;

    
};
