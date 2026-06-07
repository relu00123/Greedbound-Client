// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "GBAssetManager.generated.h"

class UGBCharacterData;
enum class EGBCharacterClassType : uint8;

struct FGameplayTag;

UCLASS()
class GREEDBOUND_API UGBAssetManager : public UAssetManager
{
    GENERATED_BODY()

public:
    virtual void            StartInitialLoading() override;

    static UGBAssetManager& Get() { return *Cast<UGBAssetManager>(GEngine->AssetManager); }

    template<typename T>
    static T*               GetAsset(const TSoftObjectPtr<T>& AssetPointer, bool bKeepInMemory = true);
};

template<typename T>
T* UGBAssetManager::GetAsset(const TSoftObjectPtr<T>& AssetPointer, bool bKeepInMemory /*= true*/)
{
    T* LoadedAsset = nullptr;
    const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

    if (AssetPath.IsValid())
    {
        LoadedAsset = AssetPointer.Get();
        if (LoadedAsset == nullptr)
        {
            LoadedAsset = Cast<T>(SynchronousLoadAsset(AssetPath));
            ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
        }

        if (LoadedAsset && bKeepInMemory)
        {
            Get.AddLoadedAsset(Cast<UObject>(LoadedAsset));
        }
    }
}
