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
    UFUNCTION(BlueprintCallable, Category = "Thesis|IK")
    static void SetFootIKC(
        USkeletalMeshComponent* mesh,
        ACharacter* character,
        bool isCrouching,
        FName anim_curve_name,
        FName foot_ikbone,
        FName root_bone,
        UPARAM(ref) FVector& foot_ik_offset,
        UPARAM(ref) FVector& foot_ik_target,
        UPARAM(ref) FRotator& foot_ik_rotator,
        float foot_height
    );

    UFUNCTION(BlueprintCallable, Category = "Thesis|IK")
    static void SetPelvisIKC(
        USkeletalMeshComponent* mesh,
        FVector foot_ik_l_target,
        FVector foot_ik_r_target,
        UPARAM(ref) float& pelvis_alpha,
        UPARAM(ref) FVector& pelvis_offset
    );
};