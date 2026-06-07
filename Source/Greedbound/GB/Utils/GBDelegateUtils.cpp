// GBDelegateUtils.cpp
#include "GB/Utils/GBDelegateUtils.h"
#include "UObject/UnrealType.h"
#include "UObject/ScriptDelegates.h"

DEFINE_LOG_CATEGORY_STATIC(LogGBDelegateUtils, Log, All);

namespace GB::DelegateUtils
{
    bool BindScriptDelegateTo(UObject* Target, FName FunctionName, FMulticastScriptDelegate& Dest)
    {
        if (!IsValid(Target) || !FunctionName.IsValid())
        {
            UE_LOG(LogGBDelegateUtils, Warning, TEXT("BindScriptDelegateTo: Invalid Target or FunctionName."));
            return false;
        }

        // 유효한 UFUNCTION인지 가볍게 체크(선택 사항)
        if (UFunction* Func = Target->FindFunction(FunctionName); Func == nullptr)
        {
            UE_LOG(LogGBDelegateUtils, Warning, TEXT("BindScriptDelegateTo: Function '%s' not found on %s."),
                *FunctionName.ToString(), *GetNameSafe(Target));
            // 함수가 나중에 생성/바인딩될 수 있으면 이 경고만 남기고 계속 가도 되지만,
            // 여기서는 안전하게 false 반환
            return false;
        }

        FScriptDelegate Delegate;
        Delegate.BindUFunction(Target, FunctionName);

        if (Dest.Contains(Delegate))
        {
            // 이미 바인딩됨
            return true;
        }

        Dest.Add(Delegate);
        return true;
    }

    void UnbindScriptDelegateFrom(UObject* Target, FName FunctionName, FMulticastScriptDelegate& Dest)
    {
        if (!IsValid(Target) || !FunctionName.IsValid())
        {
            UE_LOG(LogGBDelegateUtils, Verbose, TEXT("UnbindScriptDelegateFrom: Invalid Target or FunctionName."));
            return;
        }

        FScriptDelegate Delegate;
        Delegate.BindUFunction(Target, FunctionName);
        Dest.Remove(Delegate);
    }
}
