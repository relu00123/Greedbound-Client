// Fill out your copyright notice in the Description page of Project Settings.


#include "GB/WorldActors/SpawnPoint/GBCharacterSpawnPoint.h"

 
AGBCharacterSpawnPoint::AGBCharacterSpawnPoint(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = false; 
}

void AGBCharacterSpawnPoint::MakeSpawnTransforms(int32 NumPlayers, TArray<FTransform>& Out) const
{
    Out.Reset();
    const int32 N = FMath::Clamp(NumPlayers, 0, MaxSlots);
    if (N <= 0) return;

    const FTransform Center = GetActorTransform();

    for (int32 i = 0; i < N; ++i)
    {
        const float t = (float)i / (float)N;
        const float BaseDeg = 360.f * t;
        const float Deg = BaseDeg + FMath::FRandRange(-AngularJitterDeg, AngularJitterDeg);
        const float Rad = FMath::DegreesToRadians(Deg);

        const float R = SlotRadius * (1.f + FMath::FRandRange(-RadialJitterRatio, RadialJitterRatio));
        const FVector Local(R * FMath::Cos(Rad), R * FMath::Sin(Rad), 0.f);

        const FVector WorldLoc = Center.TransformPosition(Local);

        // APlayerStart의 앞방향을 그대로 사용(필요하면 LookAt 등으로 교체)
        const FRotator WorldRot = Center.GetRotation().Rotator();
        Out.Add(FTransform(WorldRot, WorldLoc, FVector::OneVector));
    }
}

void AGBCharacterSpawnPoint::OnConstruction(const FTransform& Transform)
{
#if WITH_EDITOR
    // 에디터 미리보기: 슬롯 원과 균등 각도 배치 점(지터 미적용 프리뷰)
    const int32 N = FMath::Max(1, MaxSlots);
    const FVector C = GetActorLocation();

    DrawDebugCircle(GetWorld(), C, SlotRadius, 32, FColor::Yellow, false, 0.f, 0, 2.f,
        FVector(1, 0, 0), FVector(0, 1, 0), false);

    for (int32 i = 0; i < N; ++i)
    {
        const float Deg = 360.f * (float)i / (float)N;
        const float Rad = FMath::DegreesToRadians(Deg);
        const FVector P = C + FVector(SlotRadius * FMath::Cos(Rad), SlotRadius * FMath::Sin(Rad), 0.f);
        DrawDebugSphere(GetWorld(), P, 12.f, 10, FColor::Cyan, false, 0.f, 0, 1.5f);
    }
#endif
}
