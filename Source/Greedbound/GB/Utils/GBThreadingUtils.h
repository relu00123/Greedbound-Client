#pragma once
#include "CoreMinimal.h"
#include "Async/Async.h"

namespace GB::ThreadingUtils
{
    inline void RunOnGameThread(TFunction<void()> Fn)
    {
        if (IsInGameThread()) 
        { 
            Fn(); 
        }
        else 
        { 
            AsyncTask(ENamedThreads::GameThread, MoveTemp(Fn)); 
        }
    }

    template<typename TCallable>
    inline void RunOnGameThreadSafe(TWeakObjectPtr<const UObject> WeakGuard, TCallable&& Fn)
    {
        if (IsInGameThread())
        {
            if (WeakGuard.IsValid()) { Forward<TCallable>(Fn)(); }
            return;
        }
        AsyncTask(ENamedThreads::GameThread, [WeakGuard, F = Forward<TCallable>(Fn)]() mutable
            {
                if (WeakGuard.IsValid()) { F(); }
            });
    }
}
