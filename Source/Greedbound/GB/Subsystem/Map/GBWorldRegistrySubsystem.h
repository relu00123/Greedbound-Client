// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/Map/GBWorldCatalogData.h"
#include "Define/Map/GBWorldTypes.h"
#include "GBWorldRegistrySubsystem.generated.h"

UCLASS()
class GREEDBOUND_API UGBWorldRegistrySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:
    void                SetWorldCatalog(TSoftObjectPtr<UGBWorldCatalogData> InCatalog); 

    UFUNCTION(BlueprintPure)
    EGBWorldCategory    GetCategoryByWorld(const UWorld* World) const;
    
private:
    UPROPERTY()
    TMap<FName, EGBWorldCategory> PackageToCategory; // Key = LongPackageName 
    
};
