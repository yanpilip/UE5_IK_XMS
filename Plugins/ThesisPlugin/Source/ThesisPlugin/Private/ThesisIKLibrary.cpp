// Fill out your copyright notice in the Description page of Project Settings.


#include "ThesisIKLibrary.h"
#include "ThesisPlugin.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "ThesisAnimInstance.h"

void UThesisIKLibrary::SetFootIKC(
	
	USkeletalMeshComponent* mesh,
	ACharacter* Character,
	bool isCrouching,
	FName anim_curve_name,
	FName foot_ikbone,
	FName root_bone,
	FVector& foot_ik_offset,
	FVector& foot_ik_target,
	FRotator& foot_ik_rotator,
	float foot_height
)
{
	if (!mesh || !mesh->GetAnimInstance() || !Character) return;

	if (mesh->GetAnimInstance()->GetCurveValue(anim_curve_name) <= 0.0f) {
		foot_ik_rotator = FRotator(0, 0, 0);
		foot_ik_offset = FVector(0, 0, 0);
		return;
	}
	FVector expected_floor_location = FVector(mesh->GetSocketLocation(foot_ikbone).X, mesh->GetSocketLocation(foot_ikbone).Y, mesh->GetSocketLocation(root_bone).Z);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(mesh->GetOwner());
	bool bHit = mesh->GetWorld()->LineTraceSingleByChannel(
		HitResult,
		expected_floor_location + FVector(0, 0, 50),
		expected_floor_location - FVector(0, 0, 60),
		ECC_Visibility,
		Params
	);
	
	if (!bHit) return;
	if (!Character->GetCharacterMovement()->IsWalkable(HitResult)) return;
	
	FVector impactPoint = FVector(HitResult.ImpactPoint);
	FVector impactNormal = FVector(HitResult.ImpactNormal);

	FVector foot = impactPoint + (impactNormal * foot_height);
	FVector base = expected_floor_location + (FVector(0, 0, 1.0f) * foot_height);

	foot_ik_target = foot - base;

	FRotator foot_ik_rotator_target = FRotator(
		FMath::RadiansToDegrees(FMath::Atan2(impactNormal.X, impactNormal.Z)) * -1.0f,  // Pitch
		0.0f,                                                                              // Yaw
		FMath::RadiansToDegrees(FMath::Atan2(impactNormal.Y, impactNormal.Z))            // Roll
	);
	if (HitResult.Distance > 100 || (isCrouching && HitResult.Distance > 80)) {
		foot_ik_target = FVector::ZeroVector;
		UThesisAnimInstance* AnimInstance = Cast<UThesisAnimInstance>(mesh->GetAnimInstance());
		if (AnimInstance)
		{
			AnimInstance->ResetIK();
		}
	}

	if (foot_ik_offset.Z > foot_ik_target.Z) {
		foot_ik_offset = FMath::VInterpTo(
			foot_ik_offset,
			foot_ik_target,
			mesh->GetWorld()->GetDeltaSeconds(),
			30.0f
		);
	}
	else {
		foot_ik_offset = FMath::VInterpTo(
			foot_ik_offset,
			foot_ik_target,
			mesh->GetWorld()->GetDeltaSeconds(),
			85.0f
		);
	}

	foot_ik_rotator = FMath::RInterpTo(
		foot_ik_rotator,
		foot_ik_rotator_target,
		mesh->GetWorld()->GetDeltaSeconds(),
		10.0f
	);

}


void UThesisIKLibrary::SetPelvisIKC(
	USkeletalMeshComponent* mesh,
	FVector foot_ik_l_target,
	FVector foot_ik_r_target,
	float& pelvis_alpha,
	FVector& pelvis_offset
)
{
	pelvis_alpha = ((mesh->GetAnimInstance()->GetCurveValue("Enable_FootIK_L")) + (mesh->GetAnimInstance()->GetCurveValue("Enable_FootIK_R"))) / 2.0f;

	if (pelvis_alpha <= 0) {
		pelvis_offset = FVector(0, 0, 0);
		return;
	}

	FVector pelvis_target = FVector::ZeroVector;
	if (foot_ik_l_target.Z < foot_ik_r_target.Z) {
		pelvis_target = foot_ik_l_target;
	}
	else {
		pelvis_target = foot_ik_r_target;
	}

	if (pelvis_offset.Z < pelvis_target.Z) {
		pelvis_offset = FMath::VInterpTo(
			pelvis_offset,
			pelvis_target,
			mesh->GetWorld()->GetDeltaSeconds(),
			10.0f
		);
	}
	else {
		pelvis_offset = FMath::VInterpTo(
			pelvis_offset,
			pelvis_target,
			mesh->GetWorld()->GetDeltaSeconds(),
			15.0f
		);
	}
}