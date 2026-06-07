// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GBDataListEntryMappingData.generated.h"

class UGBListDataObject_Base;
class UGBListEntryWidget_Base;

UCLASS()
class GREEDBOUND_API UGBDataListEntryMappingData : public UDataAsset
{
    GENERATED_BODY()

public:
    TSubclassOf<UGBListEntryWidget_Base> FindEntryWidgetClassByDataObject(UGBListDataObject_Base* InDataObject) const;

private:
    UPROPERTY(EditDefaultsOnly)
    TMap<TSubclassOf<UGBListDataObject_Base>, TSubclassOf<UGBListEntryWidget_Base>> DataObjectListEntryMap;
};
