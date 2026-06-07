// Fill out your copyright notice in the Description page of Project Settings.

#include "GBWorldRegistrySubsystem.h"

void UGBWorldRegistrySubsystem::SetWorldCatalog(TSoftObjectPtr<UGBWorldCatalogData> InCatalog)
{
    PackageToCategory.Reset();
    if (const auto* Cat = InCatalog.LoadSynchronous()) {
        for (const auto& WorldEntry : Cat->Worlds) {
            const FSoftObjectPath P = WorldEntry.World.ToSoftObjectPath();
            PackageToCategory.Add(FName(*P.GetLongPackageName()), WorldEntry.WorldCategory);
        }
    }
}

EGBWorldCategory UGBWorldRegistrySubsystem::GetCategoryByWorld(const UWorld* World) const
{
    if (!World) return EGBWorldCategory::None;
    const FName Pkg(*World->GetOutermost()->GetName());
    if (const auto* Found = PackageToCategory.Find(Pkg)) return *Found;
    return EGBWorldCategory::None;
}
