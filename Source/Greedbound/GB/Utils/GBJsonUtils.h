#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"

class FGBJsonUtils
{
public:
    static TOptional<FString> ReadOptionalString(const TSharedPtr<FJsonObject>& Obj, const TCHAR* Key);
    static TOptional<int32>   ReadOptionalInt(const TSharedPtr<FJsonObject>& Obj, const TCHAR* Key);
    static TOptional<bool>    ReadOptionalBool(const TSharedPtr<FJsonObject>& Obj, const TCHAR* Key);
    static void               ReadStringArray(const TSharedPtr<FJsonObject>& Obj, const TCHAR* Key, TArray<FString>& Out);

    template<typename T>
    static TSharedPtr<FJsonObject> StructToJsonObject(const T& StructInstance)
    {
        TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

        if (FJsonObjectConverter::UStructToJsonObject(T::StaticStruct(), &StructInstance, JsonObject.ToSharedRef(), 0, 0))
        {
            return JsonObject;
        }

        UE_LOG(LogTemp, Error, TEXT("StructToJsonObject: Failed to convert struct to JSON."));
        return nullptr;
    }

    template<typename T>
    static bool JsonObjectToStruct(const TSharedPtr<FJsonObject>& JsonObject, T& OutStruct)
    {
        if (!JsonObject.IsValid())
        {
            return false;
        }

        return FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), T::StaticStruct(), &OutStruct, 0, 0);
    }
};
