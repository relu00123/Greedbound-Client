// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonListView.h"
#include "GBCommonListView.generated.h"

class UGBDataListEntryMappingData;

UCLASS()
class GREEDBOUND_API UGBCommonListView : public UCommonListView
{
	GENERATED_BODY()

protected:
    virtual UUserWidget&                    OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable) override final;
    virtual bool                            OnIsSelectableOrNavigableInternal(UObject* FirstSelectedItem) override final;

private:
#if WITH_EDITOR	
    virtual void                            ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override final;
#endif

private:
    UPROPERTY(EditAnywhere, Category = "List View Setting")
    TObjectPtr<UGBDataListEntryMappingData> DataListEntryMapping;
};
