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
	UThesisAnimInstance* AnimInstance,
	bool isCrouching,
	FName anim_curve_name,
	FName foot_ikbone,
	FName root_bone,
	FVector& foot_ik_offset,
	FVector& foot_ik_target,
	FRotator& foot_ik_rotator
)
{
	if (!mesh || !mesh->GetAnimInstance() || !Character) return;

	if (mesh->GetAnimInstance()->GetCurveValue(anim_curve_name) <= 0.0f) {
		foot_ik_rotator = FRotator(0, 0, 0);
		foot_ik_offset = FVector(0, 0, 0);
		return;
	}
	FVector expected_floor_location = FVector(mesh->GetSocketLocation(foot_ikbone).X, mesh->GetSocketLocation(foot_ikbone).Y, mesh->GetSocketLocation(root_bone).Z);

	if (AnimInstance){ 
		AnimInstance->ResetIK(); 
	}

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(mesh->GetOwner());
	bool bHit = mesh->GetWorld()->LineTraceSingleByChannel(
		HitResult,
		expected_floor_location + AnimInstance->foot_trace_above,
		expected_floor_location - AnimInstance->foot_trace_below,
		ECC_Visibility,
		Params
	);
	
	if (!bHit) return;
	if (!Character->GetCharacterMovement()->IsWalkable(HitResult)) return;
	
	FVector impactPoint = FVector(HitResult.ImpactPoint);
	FVector impactNormal = FVector(HitResult.ImpactNormal);

	FVector foot = impactPoint + (impactNormal * AnimInstance->foot_height_c);
	FVector base = expected_floor_location + (FVector(0, 0, 1.0f) * AnimInstance->foot_height_c);

	foot_ik_target = foot - base;

	FRotator foot_ik_rotator_target = FRotator(
		FMath::RadiansToDegrees(FMath::Atan2(impactNormal.X, impactNormal.Z)) * -1.0f,  // Pitch
		0.0f,                                                                              // Yaw
		FMath::RadiansToDegrees(FMath::Atan2(impactNormal.Y, impactNormal.Z))            // Roll
	);
	if (HitResult.Distance > AnimInstance->ik_break_standing_distance || (isCrouching && HitResult.Distance > AnimInstance->ik_break_crouching_distance)) {
		foot_ik_target = FVector::ZeroVector;
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
			AnimInstance->feet_interp_speed_up
		);
	}
	else {
		foot_ik_offset = FMath::VInterpTo(
			foot_ik_offset,
			foot_ik_target,
			mesh->GetWorld()->GetDeltaSeconds(),
			AnimInstance->feet_interp_speed_down
		);
	}

	foot_ik_rotator = FMath::RInterpTo(
		foot_ik_rotator,
		foot_ik_rotator_target,
		mesh->GetWorld()->GetDeltaSeconds(),
		AnimInstance->feet_interp_speed_rotation
	);

}


void UThesisIKLibrary::SetPelvisIKC(
	USkeletalMeshComponent* mesh,
	UThesisAnimInstance* AnimInstance,
	FVector foot_ik_l_target,
	FVector foot_ik_r_target,
	float& pelvis_alpha,
	FVector& pelvis_offset
)
{
	pelvis_alpha = ((mesh->GetAnimInstance()->GetCurveValue(AnimInstance->curve_left)) + (mesh->GetAnimInstance()->GetCurveValue(AnimInstance->curve_right))) / 2.0f;

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
			AnimInstance->pelvis_interp_speed_up
		);
	}
	else {
		pelvis_offset = FMath::VInterpTo(
			pelvis_offset,
			pelvis_target,
			mesh->GetWorld()->GetDeltaSeconds(),
			AnimInstance->pelvis_interp_speed_down
		);
	}
}