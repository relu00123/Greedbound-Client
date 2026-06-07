#pragma once

#if PLATFORM_WINDOWS
#define SHORT_FILE_NAME \
        (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define SHORT_FILE_NAME \
        (strrchr(__FILE__, '/')  ? strrchr(__FILE__, '/')  + 1 : __FILE__)
#endif

/// 로그 출력용, return하지 않음
#define GB_LOG(Format, ...) \
if (UE_BUILD_SHIPPING == false) \
{ \
    UE_LOG(LogTemp, Error, TEXT("[%s] At Function: %s (%s:%d)"), \
        *FString::Printf(Format, ##__VA_ARGS__), ANSI_TO_TCHAR(__FUNCTION__), ANSI_TO_TCHAR(SHORT_FILE_NAME), __LINE__); \
} \

/// PIE 모드가 아니고 빌드가 Shipping이 아니면 로그 출력
#define GB_BACKEND_LOG(LogLevel, Format, ...) \
if (UE_BUILD_SHIPPING == false && GIsEditor == false) \
{ \
    UE_LOG(LogTemp, LogLevel, TEXT("[%s] At Function: %s (%s:%d)"), \
        *FString::Printf(Format, ##__VA_ARGS__), \
        ANSI_TO_TCHAR(__FUNCTION__), ANSI_TO_TCHAR(SHORT_FILE_NAME), __LINE__); \
}

/// ptr가 nullptr이면 로그를 출력하고 return
#define GB_NULL_CHECK(ptr) \
if(ptr == nullptr) \
{ \
    if (UE_BUILD_SHIPPING == false) \
    { \
        UE_LOG(LogTemp, Error, TEXT("[%s] is nullptr, At Function: %s (%s:%d)"), \
            TEXT(#ptr), ANSI_TO_TCHAR(__FUNCTION__), ANSI_TO_TCHAR(SHORT_FILE_NAME), __LINE__); \
    } \
    return; \
} \

// ptr가 nullptr이면 ret을 return
#define GB_NULL_CHECK_WITH_RETURN(ptr, ret) \
if(ptr == nullptr) \
{ \
    return ret; \
} \

/// UObject를 상속받는 객체포인터를 대상으로 IsValid(ptr)이 실패하면 로그를 출력하고 return
#define GB_VALID_CHECK(ptr) \
if(IsValid(ptr) == false) \
{ \
    if (UE_BUILD_SHIPPING == false) \
        { \
        const UObject* CheckedObject = static_cast<const UObject*>(ptr); \
        FString ObjectName = CheckedObject ? CheckedObject->GetName() : TEXT("null"); \
        UE_LOG(LogTemp, Error, TEXT("Invalid : [%s], Name: [%s], At Function: %s (%s:%d)"), \
        TEXT(#ptr), *ObjectName, ANSI_TO_TCHAR(__FUNCTION__), ANSI_TO_TCHAR(SHORT_FILE_NAME), __LINE__); \
        } \
    return; \
} \

/// UObject를 상속받는 객체포인터를 대상으로 IsValid(ptr)이 실패하면 로그를 출력하고 ret을 return
#define GB_VALID_CHECK_WITH_RETURN(ptr, ret) \
if(IsValid(ptr) == false) \
{ \
    if (UE_BUILD_SHIPPING == false) \
        { \
        const UObject* CheckedObject = static_cast<const UObject*>(ptr); \
        FString ObjectName = CheckedObject ? CheckedObject->GetName() : TEXT("null"); \
        UE_LOG(LogTemp, Error, TEXT("Invalid : [%s], Name: [%s], At Function: %s (%s:%d)"), \
        TEXT(#ptr), *ObjectName, ANSI_TO_TCHAR(__FUNCTION__), ANSI_TO_TCHAR(SHORT_FILE_NAME), __LINE__); \
        } \
    return ret; \
} \

/// 인자로 들어가는 조건이 false라면 로그를 출력하고 return
#define GB_CONDITION_CHECK(condition) \
if(condition == false) \
{ \
    if (UE_BUILD_SHIPPING == false) \
    { \
        UE_LOG(LogTemp, Error, TEXT("[%s] is false!, At Function: %s (%s:%d)"), \
            TEXT(#condition), ANSI_TO_TCHAR(__FUNCTION__), ANSI_TO_TCHAR(SHORT_FILE_NAME), __LINE__); \
    } \
    return; \
} \

/// 인자로 들어가는 조건이 false라면 로그를 출력하고 ret을 return
#define GB_CONDITION_CHECK_WITH_RETURN(condition, ret) \
if(condition == false) \
{ \
    if (UE_BUILD_SHIPPING == false) \
    { \
        UE_LOG(LogTemp, Error, TEXT("[%s] is false!, At Function: %s (%s:%d)"), \
            TEXT(#condition), ANSI_TO_TCHAR(__FUNCTION__), ANSI_TO_TCHAR(SHORT_FILE_NAME), __LINE__); \
    } \
    return ret; \
} \

/// TEST용 임시로그 출력
#define GB_TESTLOG(Format, ...) \
if (UE_BUILD_SHIPPING == false) \
{ \
    UE_LOG(LogTemp, Error, TEXT("TestLog : [%s] At Function: %s (%s:%d)"), \
        *FString::Printf(Format, ##__VA_ARGS__), ANSI_TO_TCHAR(__FUNCTION__), ANSI_TO_TCHAR(SHORT_FILE_NAME), __LINE__); \
}
