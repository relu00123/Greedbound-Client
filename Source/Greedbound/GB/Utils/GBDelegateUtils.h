// GBDelegateUtils.h
#pragma once

#include "CoreMinimal.h"

/**
 * 범용 ScriptDelegate 바인딩 유틸
 * - UObject + UFunction(FName)로 멀티캐스트 델리게이트에 안전 바인딩/언바인딩
 * - 중복 바인딩 방지
 */
namespace GB::DelegateUtils
{
    /** UObject(Target)의 UFUNCTION(FunctionName)을 Dest(FMulticastScriptDelegate)에 바인딩. 이미 있으면 추가하지 않음. */
    bool BindScriptDelegateTo(UObject* Target, FName FunctionName, FMulticastScriptDelegate& Dest);

    /** UObject(Target)의 UFUNCTION(FunctionName)을 Dest(FMulticastScriptDelegate)에서 제거. */
    void UnbindScriptDelegateFrom(UObject* Target, FName FunctionName, FMulticastScriptDelegate& Dest);
}
