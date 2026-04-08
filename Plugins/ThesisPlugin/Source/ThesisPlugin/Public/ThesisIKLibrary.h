#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ThesisIKLibrary.generated.h"

class UThesisAnimInstance;
class ACharacter;
class USkeletalMeshComponent;
UCLASS()
class THESISPLUGIN_API UThesisIKLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	

public:
    static FRotator CalculateFootRotation(const FVector& impactNormal);

    static FVector CalculateFootTarget(
        const FVector& impactPoint,
        const FVector& impactNormal,
        const FVector& efl,
        float fh
    );

    static float CalculatePelvisAlpha(float left, float right);

    static FVector CalculatePelvisTarget(FVector left, FVector right);

    static bool ShouldBreakIK(float distance, float standingLimit, float crouchingLimit, bool isCrouching);

    static float SelectInterpSpeed(float currentZ, float targetZ, float up, float down);

    UFUNCTION(BlueprintCallable, Category = "Thesis|IK")
    static void SetFootIKC(
        USkeletalMeshComponent* mesh,
        ACharacter* character,
        UThesisAnimInstance* AnimInstance,
        bool isCrouching,
        FName anim_curve_name,
        FName foot_ikbone,
        FName root_bone,
        UPARAM(ref) FVector& foot_ik_offset,
        UPARAM(ref) FVector& foot_ik_target,
        UPARAM(ref) FRotator& foot_ik_rotator 
    );

    UFUNCTION(BlueprintCallable, Category = "Thesis|IK")
    static void SetPelvisIKC(
        USkeletalMeshComponent* mesh,
        UThesisAnimInstance* AnimInstance,
        FVector foot_ik_l_target,
        FVector foot_ik_r_target,
        UPARAM(ref) float& pelvis_alpha,
        UPARAM(ref) FVector& pelvis_offset
    );
};