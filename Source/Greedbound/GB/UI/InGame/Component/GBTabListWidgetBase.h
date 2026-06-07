// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "GBTabListWidgetBase.generated.h"

class UGBCommonButtonBase;
class IWidgetCompilerLog;

/*
*   옵션창에서 Vedio, Audio 등의 상단 탭 표시해주는 UI
*/
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class GREEDBOUND_API UGBTabListWidgetBase : public UCommonTabListWidgetBase
{
	GENERATED_BODY()

public:
    void                                RequestRegisterTab(const FName& InTabId, const FText& InTabDisplayName);

private:
#if WITH_EDITOR	
    virtual void                        ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override final;
#endif
	
private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tab List Setting", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UGBCommonButtonBase>    TabButtonEntryWidgetClass;
};
