// Fill out your copyright notice in the Description page of Project Settings.


#include "ThesisIKLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UThesisIKLibrary::FootPlacement(
    const UObject * WorldContextObject,
    const FVector & LfootLocation,
    const FVector & RfootLocation,
    const FVector & rootLocation,
    bool& OutShouldTraceL,
    bool& OutShouldTraceR
)
{
    UWorld* World = WorldContextObject->GetWorld();
    OutShouldTraceL = true;
    OutShouldTraceR = true;
    if (!World) return;
    if (LfootLocation.Z > rootLocation.Z + 10.0f) {
        //DrawDebugSphere(World, LfootLocation, 3.0f, 12, FColor::Red, false, 15.0f);
        //GEngine->AddOnScreenDebugMessage(1, 10, FColor::Yellow, FString::Printf(TEXT("Release left")));
        OutShouldTraceL = false;
    }
    if (RfootLocation.Z > rootLocation.Z + 10.0f) {
        //DrawDebugSphere(World, RfootLocation, 3.0f, 12, FColor::Blue, false, 15.0f);
        //GEngine->AddOnScreenDebugMessage(1, 10, FColor::Yellow, FString::Printf(TEXT("Release right")));
        OutShouldTraceR = false;
    }
}


void UThesisIKLibrary::ComputeFootOffsetAndRotation(
    const UObject* WorldContextObject,
    const FVector& LfootLocation,
    const FVector& RfootLocation,
    const FVector& rootLocation,
    const bool& ShouldTraceL,
    const bool& ShouldTraceR,
    FVector& OutOffsetL,
    FRotator& OutRotL,
    FVector& OutOffsetR,
    FRotator& OutRotR,
    FVector& OutOffsetPel,
    FHitResult& OutLHit,
    FHitResult& OutRHit,
    bool& OutIfHit,
    bool& OutShouldRagdoll
)
{
    FHitResult LHit;
    FHitResult RHit;
    bool bHitL = false;
    bool bHitR = false;
    OutShouldRagdoll = false;
    /// A
    FVector LstartVec = LfootLocation + FVector(0, 0, 30);
    FVector LendVec = LfootLocation - FVector(0, 0, 50);
    UWorld* World = WorldContextObject->GetWorld();
    if (!World) return;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(nullptr);

    bHitL = World->SweepSingleByChannel(
        LHit,
        LstartVec,
        LendVec,
        FQuat::Identity,
        ECC_Visibility,
        FCollisionShape::MakeSphere(3.0f),
        Params
    );

    FVector LImpactPoint = FVector(0, 0, 0);
    FVector LImpactNormal = FVector(0, 0, 0);

    if (bHitL)
    {
        LImpactPoint = LHit.ImpactPoint;
        LImpactNormal = LHit.ImpactNormal;
        OutLHit = LHit;
    }
    /// B
    FVector RstartVec = RfootLocation + FVector(0, 0, 30);
    FVector RendVec = RfootLocation - FVector(0, 0, 50);

    bHitR = World->SweepSingleByChannel(
        RHit,
        RstartVec,
        RendVec,
        FQuat::Identity,
        ECC_Visibility,
        FCollisionShape::MakeSphere(3.0f),
        Params
    );

    FVector RImpactPoint = FVector(0, 0, 0);
    FVector RImpactNormal = FVector(0, 0, 0);

    OutOffsetL = FVector(0, 0, 0);
    OutRotL = FRotator(0, 0, 0);
    OutOffsetR = FVector(0, 0, 0);
    OutRotR = FRotator(0, 0, 0);

    if (((ShouldTraceL && !bHitL) || (ShouldTraceR && !bHitR)) || (ShouldTraceL && (rootLocation.Z - LHit.ImpactPoint.Z > 50.0f)) || (ShouldTraceR && (rootLocation.Z - RHit.ImpactPoint.Z > 50.0f))) {
        //GEngine->AddOnScreenDebugMessage(1, 10, FColor::Yellow, FString::Printf(TEXT("%d %d | %f - L %f R %f"), (ShouldTraceL && !bHitL), (ShouldTraceR && !bHitR), rootLocation.Z,  LHit.ImpactPoint.Z, RHit.ImpactPoint.Z));
        OutOffsetL = FVector(0, 0, 0);
        OutRotL = FRotator(0, 0, 0);
        OutOffsetR = FVector(0, 0, 0);
        OutRotR = FRotator(0, 0, 0);
        OutOffsetPel = FVector(0, 0, 0);
        OutIfHit = false;
        
        OutShouldRagdoll = true;
        return;
    }
        OutIfHit = true;
        RImpactPoint = RHit.ImpactPoint;
        RImpactNormal = RHit.ImpactNormal;
        OutRHit = RHit;
        float PelvisL = 0;
        float PelvisR = 0;


        if (ShouldTraceL) {
            OutOffsetL.Z = LHit.Location.Z - LfootLocation.Z;
            PelvisL = LImpactPoint.Z - rootLocation.Z;
        }
        else {
            PelvisL = FLT_MAX;
        }
        if (ShouldTraceR) {
            OutOffsetR.Z = RHit.Location.Z - RfootLocation.Z;
            PelvisR = RImpactPoint.Z - rootLocation.Z;
        }
        else {
            PelvisR = FLT_MAX;
        }

        OutOffsetPel.Z = FMath::Min(PelvisL, PelvisR);
        if (OutOffsetPel.Z == FLT_MAX)
        {
            OutOffsetPel.Z = 0.f;
        }
        GEngine->AddOnScreenDebugMessage(1, 10, FColor::Yellow, FString::Printf(TEXT("%f"), OutOffsetPel.Z));

        if (PelvisL > PelvisR) {
            OutOffsetL.Z -= OutOffsetPel.Z;
        }
        else {
            OutOffsetR.Z -= OutOffsetPel.Z;
        }

        //OutOffsetL.Z = FMath::Clamp(OutOffsetL.Z, -40.f, 40.f);
        //OutOffsetR.Z = FMath::Clamp(OutOffsetR.Z, -40.f, 40.f);

        //GEngine->AddOnScreenDebugMessage(1, 10, FColor::Yellow, FString::Printf(TEXT("LImpactPointZ: %f . RootZ: %f . OutOffsetLZ: %f . OutOffsetRZ: %f. OutOffsetPel: %f"), LImpactPoint.Z, rootLocation.Z, OutOffsetL.Z, OutOffsetR.Z, OutOffsetPel.Z));
        //OutOffset = FVector(0, 0, (AImpactPoint.Z - rootLocation.Z));

        //GEngine->AddOnScreenDebugMessage(1, 10, FColor::Yellow, FString::Printf(TEXT("ImpactPointZ: %f . RootZ: %f"), LImpactPoint.Z, rootLocation.Z));

        /// L
        FVector N = LHit.ImpactNormal.GetSafeNormal();
        float PitchDeg = FMath::RadiansToDegrees(FMath::Atan2(-N.X, N.Z));
        float RollDeg = FMath::RadiansToDegrees(FMath::Atan2(N.Y, N.Z));
        OutRotL = FRotator(PitchDeg, 0.f, RollDeg);

        /// R
        N = RHit.ImpactNormal.GetSafeNormal();
        PitchDeg = FMath::RadiansToDegrees(FMath::Atan2(-N.X, N.Z));
        RollDeg = FMath::RadiansToDegrees(FMath::Atan2(N.Y, N.Z));
        OutRotR = FRotator(PitchDeg, 0.f, RollDeg);
        //OutRot = FRotator((FMath::Atan2(Hit.Normal.X, Hit.Normal.Z) * -1.0f), 0, FMath::Atan2(Hit.Normal.Y, Hit.Normal.Z));
}
