// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GBOptionsDataRegistry.generated.h"

class UGBListDataObject_Collection;
class UGBListDataObject_Base;
/*
 * 옵션창 구성용 클래스
 */
UCLASS()
class GREEDBOUND_API UGBOptionsDataRegistry : public UObject
{
	GENERATED_BODY()
	
public:
    /// Gets called by options screen right after the object of type UOptionsDataRegistry is created.
    void                                                InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer);
    const TArray<UGBListDataObject_Collection*>&        GetRegisteredOptionsTabCollections() const { return RegisteredOptionsTabCollections; }
    TArray<UGBListDataObject_Base*>                     GetListSourceItemBySelectedTabID(const FName& InSelectedTabID) const;

private:
    void                                                FindChildListDataRecursively(UGBListDataObject_Base* InParentData, TArray<UGBListDataObject_Base*>& OutChildDataList) const;

    void                                                InitGameplayCollectionTab();
    void                                                InitAudioCollectionTab();
    void                                                InitVideoCollectionTab();
    void                                                InitControlCollectionTab(ULocalPlayer* InOwningLocalPlayer);

private:
    UPROPERTY(Transient)
    TArray<TObjectPtr<UGBListDataObject_Collection>>    RegisteredOptionsTabCollections;
};
