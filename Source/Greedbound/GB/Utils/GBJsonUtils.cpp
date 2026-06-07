// GBJsonUtils.cpp
#include "GBJsonUtils.h"
#include "Dom/JsonValue.h"
#include "Misc/Char.h"
#include "Math/UnrealMathUtility.h"

namespace
{
    // 공통: 필드 유효성/존재/NULL 체크 후 JsonValue 반환
    static TSharedPtr<FJsonValue> GetFieldIfValid(const TSharedPtr<FJsonObject>& Obj, const TCHAR* Key)
    {
        if (!Obj.IsValid() || !Key || !Obj->HasField(Key))
        {
            return nullptr;
        }
        const TSharedPtr<FJsonValue> Val = Obj->TryGetField(Key);
        if (!Val.IsValid() || Val->IsNull())
        {
            return nullptr;
        }
        return Val;
    }

    // "true"/"false"/"1"/"0"/"yes"/"no" 등 문자열을 bool로 파싱
    static bool TryParseBoolFromString(const FString& In, bool& Out)
    {
        FString S = In;
        S.TrimStartAndEndInline();
        S.ToLowerInline();

        if (S == TEXT("true") || S == TEXT("1") || S == TEXT("yes") || S == TEXT("y") || S == TEXT("on"))
        {
            Out = true;
            return true;
        }
        if (S == TEXT("false") || S == TEXT("0") || S == TEXT("no") || S == TEXT("n") || S == TEXT("off"))
        {
            Out = false;
            return true;
        }
        return false;
    }

    // 문자열을 정수(int32)로 파싱 (기본 10진수)
    static bool TryParseIntFromString(const FString& In, int32& Out)
    {
        // LexTryParseString은 float/double/int 등에 대해 동작함.
        return LexTryParseString(Out, *In);
    }
}

TOptional<FString> FGBJsonUtils::ReadOptionalString(const TSharedPtr<FJsonObject>& Obj, const TCHAR* Key)
{
    const TSharedPtr<FJsonValue> Val = GetFieldIfValid(Obj, Key);
    if (!Val.IsValid())
    {
        return TOptional<FString>(); // 없음 or null
    }

    // 문자열 타입
    if (Val->Type == EJson::String)
    {
        FString Out;
        if (Obj->TryGetStringField(Key, Out))
        {
            return TOptional<FString>(MoveTemp(Out));
        }
        return TOptional<FString>();
    }

    // 문자열이 아닌데, 숫자/불리언 등을 문자열로 받아도 괜찮다면 아래 허용
    // 필요 없으면 이 블록 제거
    {
        // 안전하게 ToString() 사용 (JSON 원시 문자열과는 다를 수 있음에 유의)
        return TOptional<FString>(Val->AsString());
    }
}

TOptional<int32> FGBJsonUtils::ReadOptionalInt(const TSharedPtr<FJsonObject>& Obj, const TCHAR* Key)
{
    const TSharedPtr<FJsonValue> Val = GetFieldIfValid(Obj, Key);
    if (!Val.IsValid())
    {
        return TOptional<int32>(); // 없음 or null
    }

    // 숫자 타입
    if (Val->Type == EJson::Number)
    {
        double D = 0.0;
        if (Obj->TryGetNumberField(Key, D))
        {
            // 반올림해서 int32로 (원한다면 Floor/Ceil로 바꿔도 됨)
            const int32 Out = FMath::RoundToInt(D);
            return TOptional<int32>(Out);
        }
        return TOptional<int32>();
    }

    // 문자열에 숫자가 들어오는 경우 허용
    if (Val->Type == EJson::String)
    {
        FString S;
        if (Obj->TryGetStringField(Key, S))
        {
            int32 Parsed = 0;
            if (TryParseIntFromString(S, Parsed))
            {
                return TOptional<int32>(Parsed);
            }
        }
        return TOptional<int32>();
    }

    // 불리언을 0/1로 받아주고 싶다면 허용
    if (Val->Type == EJson::Boolean)
    {
        bool B = false;
        if (Obj->TryGetBoolField(Key, B))
        {
            return TOptional<int32>(B ? 1 : 0);
        }
    }

    return TOptional<int32>();
}

TOptional<bool> FGBJsonUtils::ReadOptionalBool(const TSharedPtr<FJsonObject>& Obj, const TCHAR* Key)
{
    const TSharedPtr<FJsonValue> Val = GetFieldIfValid(Obj, Key);
    if (!Val.IsValid())
    {
        return TOptional<bool>(); // 없음 or null
    }

    // 불리언 타입
    if (Val->Type == EJson::Boolean)
    {
        bool Out = false;
        if (Obj->TryGetBoolField(Key, Out))
        {
            return TOptional<bool>(Out);
        }
        return TOptional<bool>();
    }

    // 숫자 0/1 허용
    if (Val->Type == EJson::Number)
    {
        double D = 0.0;
        if (Obj->TryGetNumberField(Key, D))
        {
            return TOptional<bool>(FMath::Abs(D) > KINDA_SMALL_NUMBER);
        }
        return TOptional<bool>();
    }

    // 문자열 true/false/1/0/yes/no 허용
    if (Val->Type == EJson::String)
    {
        FString S;
        if (Obj->TryGetStringField(Key, S))
        {
            bool Parsed = false;
            if (TryParseBoolFromString(S, Parsed))
            {
                return TOptional<bool>(Parsed);
            }
        }
        return TOptional<bool>();
    }

    return TOptional<bool>();
}

void FGBJsonUtils::ReadStringArray(const TSharedPtr<FJsonObject>& Obj, const TCHAR* Key, TArray<FString>& Out)
{
    // "members" : ["u1", "u2", "u3"] 과 같은 json이 있을시
    // Out에 {"u1", "u2", "u3" } 의 TArray<FString>으로 반환해주는 유틸이다. 

    const TArray<TSharedPtr<FJsonValue>>* Arr = nullptr;
    if (Obj->TryGetArrayField(Key, Arr) && Arr)
    {
        Out.Reserve(Out.Num() + Arr->Num());
        for (const auto& E : *Arr)
        {
            FString S;
            if (E.IsValid() && E->TryGetString(S))
            {
                Out.Add(MoveTemp(S));
            }
        }
    }
}
