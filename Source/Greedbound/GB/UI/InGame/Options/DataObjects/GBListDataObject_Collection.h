// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/InGame/Options/DataObjects/GBListDataObject_Base.h"
#include "GBListDataObject_Collection.generated.h"

UCLASS()
class GREEDBOUND_API UGBListDataObject_Collection : public UGBListDataObject_Base
{
	GENERATED_BODY()

public:
    void                                        AddChildListData(UGBListDataObject_Base* InChildListData);

    virtual TArray<UGBListDataObject_Base*>     GetAllChildListData() const override final;
    virtual bool                                HasAnyChildListData() const override final;

private:
    UPROPERTY(Transient)
    TArray<TObjectPtr<UGBListDataObject_Base>>  ChildListDatas;	
};
