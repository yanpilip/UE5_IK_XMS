// Fill out your copyright notice in the Description page of Project Settings.


#include "ThesisIKLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"



void UThesisIKLibrary::ComputeFootOffsetAndRotation(
    const UObject* WorldContextObject,
	const FVector& footLocation,
	const FVector& rootLocation,
    FVector& OutOffset,
    FRotator& OutRot
)
{
    FHitResult Hit;
    bool bHit = false;
	FVector startVec = FVector(footLocation.X, footLocation.Y, (rootLocation.Z + 30.0f));
	FVector encVec = FVector(footLocation.X, footLocation.Y, (rootLocation.Z - 50.0f));

    UWorld* World = WorldContextObject->GetWorld();
    if (!World) return;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(nullptr);

    bHit = World->SweepSingleByChannel(
        Hit,
        startVec,
        encVec,
        FQuat::Identity,
        ECC_Visibility,
        FCollisionShape::MakeSphere(3.0f),
        Params
    );

    FVector ImpactPoint = FVector(0, 0, 0);
    FVector ImpactNormal = FVector(0, 0, 0);

    OutOffset = FVector(0, 0, 0);
    OutRot = FRotator(0, 0, 0);

    if (bHit)
    {
        ImpactPoint = Hit.ImpactPoint;
        ImpactNormal = Hit.ImpactNormal;

        OutOffset = FVector(0, 0, (ImpactPoint.Z - rootLocation.Z));
        //OutOffset = FVector(0, 0, Hit.ImpactPoint.Z - footLocation.Z);
        GEngine->AddOnScreenDebugMessage(1, 10, FColor::Yellow, FString::Printf(TEXT("ImpactPointZ: %f . RootZ: %f"), ImpactPoint.Z, rootLocation.Z));

        const FVector N = Hit.ImpactNormal.GetSafeNormal();

        const float PitchDeg = FMath::RadiansToDegrees(FMath::Atan2(-N.X, N.Z));
        const float RollDeg = FMath::RadiansToDegrees(FMath::Atan2(N.Y, N.Z));

        OutRot = FRotator(PitchDeg, 0.f, RollDeg);
        //OutRot = FRotator((FMath::Atan2(Hit.Normal.X, Hit.Normal.Z) * -1.0f), 0, FMath::Atan2(Hit.Normal.Y, Hit.Normal.Z));
    }
}

void UThesisIKLibrary::ComputeFootOffsetAndRotation2(
    const UObject* WorldContextObject,
    const FVector& LfootLocation,
    const FVector& RfootLocation,
    const FVector& rootLocation,
    const float CapsuleOffset,
    FVector& OutOffsetL,
    FRotator& OutRotL,
    FVector& OutOffsetR,
    FRotator& OutRotR,
    FVector& OutOffsetPel,
    FHitResult& OutLHit,
    FHitResult& OutRHit,
    bool& OutIfHit
)
{
    FHitResult LHit;
    FHitResult RHit;
    bool bHitL = false;
    bool bHitR = false;
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
    FVector RstartVec = RfootLocation + FVector(0, 0, 50);
    FVector RendVec = RfootLocation - FVector(0, 0, 60);

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

    if (!(bHitL && bHitR)) {
        OutIfHit = false;
        return;
    }
        OutIfHit = true;
        RImpactPoint = RHit.ImpactPoint;
        RImpactNormal = RHit.ImpactNormal;
        OutRHit = RHit;

        //FVector MinImpactPoint = AImpactNormal.Z < BImpactPoint.Z ? AImpactPoint : BImpactPoint;
        //OutOffsetL.Z = LImpactPoint.Z - rootLocation.Z + (LHit.ImpactNormal.GetSafeNormal() * 2.0f).Z;
        OutOffsetL.Z = LImpactPoint.Z - LfootLocation.Z;
        //OutOffsetR.Z = RImpactPoint.Z - rootLocation.Z + (RHit.ImpactNormal.GetSafeNormal() * 2.0f).Z;
        OutOffsetR.Z = RImpactPoint.Z - RfootLocation.Z;
        
        float PelvisL = LImpactPoint.Z - rootLocation.Z;
        float PelvisR = RImpactPoint.Z - rootLocation.Z;

        OutOffsetPel.Z = FMath::Min(PelvisL, PelvisR);

        if (PelvisL > PelvisR) {
            OutOffsetL.Z -= OutOffsetPel.Z;
        }
        else {
            OutOffsetR.Z -= OutOffsetPel.Z;
        }

        //OutOffsetL.Z = FMath::Clamp(OutOffsetL.Z, -40.f, 40.f);
        //OutOffsetR.Z = FMath::Clamp(OutOffsetR.Z, -40.f, 40.f);

        GEngine->AddOnScreenDebugMessage(1, 10, FColor::Yellow, FString::Printf(TEXT("LImpactPointZ: %f . RootZ: %f . OutOffsetLZ: %f . OutOffsetRZ: %f. OutOffsetPel: %f"), LImpactPoint.Z, rootLocation.Z, OutOffsetL.Z, OutOffsetR.Z, OutOffsetPel.Z));
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




void UThesisIKLibrary::ComputeFootOffsetAndRotation3(
    const UObject* WorldContextObject,
    const FVector& footLocation,
    const FVector& rootLocation,
    FVector& OutOffset,
    FRotator& OutRot
)
{
    FHitResult Hit;
    bool bHit = false;
    FVector startVec = FVector(footLocation.X, footLocation.Y + 5.0f, (rootLocation.Z + 50.0f));
    FVector encVec = FVector(footLocation.X, footLocation.Y + 5.0f, (rootLocation.Z - 50.0f));

    UWorld* World = WorldContextObject->GetWorld();
    if (!World) return;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Cast<AActor>(WorldContextObject));

    bHit = World->SweepSingleByChannel(
        Hit,
        startVec,
        encVec,
        FQuat::Identity,
        ECC_Visibility,
        FCollisionShape::MakeSphere(3.0f),
        Params
    );

    FVector ImpactPoint = FVector(0, 0, 0);
    FVector ImpactNormal = FVector(0, 0, 0);

    OutOffset = FVector(0, 0, 0);
    OutRot = FRotator(0, 0, 0);

    ImpactPoint = Hit.Location;
    ImpactNormal = Hit.ImpactNormal;

    OutOffset = ImpactPoint;
    //OutOffset = FVector(0, 0, Hit.ImpactPoint.Z - footLocation.Z);
    GEngine->AddOnScreenDebugMessage(1, 10, FColor::Yellow, FString::Printf(TEXT("ImpactPointZ: %f . RootZ: %f"), ImpactPoint.Z, rootLocation.Z));

    const FVector N = Hit.ImpactNormal.GetSafeNormal();

    const float PitchDeg = FMath::RadiansToDegrees(FMath::Atan2(-N.X, N.Z));
    const float RollDeg = FMath::RadiansToDegrees(FMath::Atan2(N.Y, N.Z));

    OutRot = FRotator(PitchDeg, 0.f, RollDeg);
    //OutRot = FRotator((FMath::Atan2(Hit.Normal.X, Hit.Normal.Z) * -1.0f), 0, FMath::Atan2(Hit.Normal.Y, Hit.Normal.Z));
    
}