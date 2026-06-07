// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_Detect.h"
#include "Define/GBAI.h"
#include "AIController.h"
#include "Interface/Monster/GBMonsterBaseInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Data/GBMonsterBaseData.h"
#include "Define/GBDefine.h"
#include "Define/GBCollision.h"
#include "Engine/OverlapResult.h"

UBTService_Detect::UBTService_Detect(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    NodeName = TEXT("Detect");
    Interval = 1.f;

    TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_Detect, TargetKey), AActor::StaticClass());
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    GB_NULL_CHECK(ControllingPawn);

    FVector Center = ControllingPawn->GetActorLocation();
    UWorld* World = ControllingPawn->GetWorld();
    GB_VALID_CHECK(World);

    TScriptInterface<IGBMonsterBaseInterface> MonsterBaseInterface = ControllingPawn;
    GB_NULL_CHECK(MonsterBaseInterface);

    const FMonsterPerceptionData& PerceptionData = MonsterBaseInterface->GetMonsterBaseData()->PerceptionData;

    const float SightRadius = PerceptionData.SightRadius;
    const float FOV = PerceptionData.FieldOfView;
    const float LoseSightRadius = PerceptionData.LoseSightRadius;
    const float RetentionTime = PerceptionData.TargetRetentionTime;

    TArray<FOverlapResult> OverlapResults; 
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(Detect), false, ControllingPawn); 
    const bool bResult = World->OverlapMultiByChannel(OverlapResults, Center, FQuat::Identity, GBCOLLISIONCHANNEL_MONSTERMESHTRACE, FCollisionShape::MakeSphere(SightRadius), QueryParams);

    AActor* NearestTarget = nullptr;
    float NearestDistanceSqr = FLT_MAX;

    UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
    GB_NULL_CHECK(BBComp);

    if (bResult)
    {
        for (const FOverlapResult& Result : OverlapResults)
        {
            AActor* Target = Result.GetActor();
            if (Target == nullptr || Target == ControllingPawn)
            {
                continue;
            }

            // 추격 중인 Target이 없던 경우에는 부채꼴로 탐지. 있는 경우에는 전방위 탐지
            if (BBComp->GetValueAsObject(TargetKey.SelectedKeyName) == nullptr)
            {
                const FVector DirectionToTarget = (Target->GetActorLocation() - Center).GetSafeNormal();
                const float Dot = FVector::DotProduct(ControllingPawn->GetActorForwardVector(), DirectionToTarget);
                const float HalfFOV = FOV * 0.5f;
                const float CosHalfFov = FMath::Cos(FMath::DegreesToRadians(HalfFOV));

                if (Dot < CosHalfFov)
                {
                    continue;
                }
            }

            const float DistSqr = FVector::DistSquared(Target->GetActorLocation(), Center);
            if (DistSqr < NearestDistanceSqr)
            {
                NearestDistanceSqr = DistSqr;
                NearestTarget = Target;
            }
        }
    }

    const float CurrentTime = World->GetTimeSeconds();

    if (BBComp)
    {
        if (NearestTarget)
        {
            BBComp->SetValueAsObject(TargetKey.SelectedKeyName, NearestTarget);
            BBComp->SetValueAsFloat(LastSensedTimeKey.SelectedKeyName, CurrentTime);
        }
        else
        {
            const bool bHasTarget = BBComp->GetValueAsObject(TargetKey.SelectedKeyName) != nullptr;
            const float LastSensedTime = BBComp->GetValueAsFloat(LastSensedTimeKey.SelectedKeyName);

            if (bHasTarget && (CurrentTime - LastSensedTime) > RetentionTime)
            {
                BBComp->SetValueAsObject(TargetKey.SelectedKeyName, nullptr);
            }
        }
    }

#if WITH_EDITOR
    const FVector Forward = ControllingPawn->GetActorForwardVector();
    const float HalfFOVRad = FMath::DegreesToRadians(FOV * 0.5f);

    const FVector RightDir = Forward.RotateAngleAxis(FOV * 0.5f, FVector::UpVector);
    const FVector LeftDir = Forward.RotateAngleAxis(-FOV * 0.5f, FVector::UpVector);

    DrawDebugLine(World, Center, Center + RightDir * SightRadius, FColor::Yellow, false, Interval, 0, 1.0f);
    DrawDebugLine(World, Center, Center + LeftDir * SightRadius, FColor::Yellow, false, Interval, 0, 1.0f);

    DrawDebugLine(World, Center, Center + Forward * SightRadius, FColor::Red, false, Interval, 0, 1.0f);
#endif
}
