// Fill out your copyright notice in the Description page of Project Settings.

#include "GBTabListWidgetBase.h"
#include "Editor/WidgetCompilerLog.h"
#include "UI/InGame/Component/GBCommonButtonBase.h"

void UGBTabListWidgetBase::RequestRegisterTab(const FName& InTabId, const FText& InTabDisplayName)
{
    if (GetRegisteredTabsByID().Contains(InTabId))
    {
        return;
    }

    RegisterTab(InTabId, TabButtonEntryWidgetClass, nullptr);

    UGBCommonButtonBase* FoundedButton = Cast<UGBCommonButtonBase>(GetTabButtonBaseByID(InTabId));
    if (FoundedButton)
    {
        FoundedButton->SetButtonText(InTabDisplayName);
    }
}

#if WITH_EDITOR	
void UGBTabListWidgetBase::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
    Super::ValidateCompiledDefaults(CompileLog);

    if (TabButtonEntryWidgetClass == nullptr)
    {
        CompileLog.Error(FText::FromString(TEXT("TabButtonEntryWidgetClass has no valid entry specified.") +
            GetClass()->GetName() +
            TEXT("needs a valid entry widget class to function properly")
        ));
    }
}
#endif
