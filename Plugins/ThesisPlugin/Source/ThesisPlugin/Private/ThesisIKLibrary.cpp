// Fill out your copyright notice in the Description page of Project Settings.


#include "ThesisIKLibrary.h"
#include "ThesisPlugin.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "ThesisAnimInstance.h"

FRotator UThesisIKLibrary::CalculateFootRotation(const FVector& impactNormal)
{
	return FRotator(
		FMath::RadiansToDegrees(FMath::Atan2(impactNormal.X, impactNormal.Z)) * -1.0f,
		0.0f,
		FMath::RadiansToDegrees(FMath::Atan2(impactNormal.Y, impactNormal.Z))
	);
}

FVector UThesisIKLibrary::CalculateFootTarget(const FVector& impactPoint, const FVector& impactNormal, const FVector& efl, const float fh)
{
	FVector foot = impactPoint + (impactNormal * fh);
	FVector base = efl + (FVector(0, 0, 1.0f) * fh);

	return (foot - base);
}

bool UThesisIKLibrary::ShouldBreakIK(float distance, float standingLimit, float crouchingLimit, bool isCrouching)
{
	return distance > (isCrouching ? crouchingLimit : standingLimit);
}

float UThesisIKLibrary::SelectInterpSpeed(float currentZ, float targetZ, float up, float down)
{
	return (currentZ > targetZ) ? up : down;
}

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
	if (!mesh || !Character || !AnimInstance) return;
	if (AnimInstance->ThesisDebugMode) {
		GEngine->AddOnScreenDebugMessage(7, 15.0f, FColor::Yellow, TEXT("===SetFootIKC Start===\nSetFootIKC -> (!mesh || !Character || !AnimInstance) check: True"));
	}
	UAnimInstance* AnimInst = Cast<UAnimInstance>(AnimInstance);
	if (!AnimInst) return;
	if (AnimInstance->ThesisDebugMode) {
		GEngine->AddOnScreenDebugMessage(8, 15.0f, FColor::Yellow, TEXT("SetFootIKC -> (AnimInst Cast<UAnimInstance>(AnimInstance)) check: True"));
	}
	UWorld* World = mesh->GetWorld();
	if (!World) return;
	if (AnimInstance->ThesisDebugMode) {
		GEngine->AddOnScreenDebugMessage(9, 15.0f, FColor::Yellow, TEXT("SetFootIKC -> (World) check: True"));
	}

	if (!Character->GetCharacterMovement()) return;
	if (AnimInstance->ThesisDebugMode) {
		GEngine->AddOnScreenDebugMessage(10, 15.0f, FColor::Yellow, TEXT("SetFootIKC -> (!Character->GetCharacterMovement()) check: True"));
	}

	float cur_val = AnimInst->GetCurveValue(anim_curve_name);
	if (AnimInstance->force_curve_value) {
		cur_val = 1.0f;
	}
	if (cur_val <= 0.0f) {
		foot_ik_rotator = FRotator::ZeroRotator;
		foot_ik_offset = FVector::ZeroVector;
		return;
	}
	if (AnimInstance->ThesisDebugMode) {
		GEngine->AddOnScreenDebugMessage(11, 15.0f, FColor::Yellow, TEXT("SetFootIKC -> (AnimInst->GetCurveValue(anim_curve_name) > 0.0f) check: True"));
	}

	FVector lateral_offset = Character->GetActorRightVector() * AnimInstance->ik_stance_width_offset;
	if (foot_ikbone == AnimInstance->foot_ik_l_name) {
		lateral_offset *= -1.0f;
	}

	FVector expected_floor_location = FVector(mesh->GetSocketLocation(foot_ikbone).X, mesh->GetSocketLocation(foot_ikbone).Y, mesh->GetSocketLocation(root_bone).Z) + lateral_offset;

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
	if (AnimInstance->ThesisDebugMode) {
		GEngine->AddOnScreenDebugMessage(12, 15.0f, FColor::Yellow, TEXT("SetFootIKC -> (bHit) check: True"));
		DrawDebugSphere(World, expected_floor_location + AnimInstance->foot_trace_above, 3.0f, 6, FColor::Red, false, -1.0f, 0, 2.0f);
		DrawDebugSphere(World, expected_floor_location - AnimInstance->foot_trace_below, 3.0f, 6, FColor::Blue, false, -1.0f, 0, 2.0f);
		DrawDebugSphere(World, HitResult.ImpactPoint, 3.0f, 6, FColor::Purple, false, -1.0f, 0, 2.0f);
		DrawDebugLine(World, expected_floor_location + AnimInstance->foot_trace_above, expected_floor_location - AnimInstance->foot_trace_below, FColor::Green, false, -1.0f, 0, 1.5f);
	}
	if (!Character->GetCharacterMovement()->IsWalkable(HitResult)) return;
	if (AnimInstance->ThesisDebugMode) {
		GEngine->AddOnScreenDebugMessage(13, 15.0f, FColor::Yellow, TEXT("SetFootIKC -> (!Character->GetCharacterMovement()->IsWalkable(HitResult)) check: True\n===SetFootIKC End==="));
	}

	foot_ik_target = CalculateFootTarget(HitResult.ImpactPoint, HitResult.ImpactNormal, expected_floor_location, AnimInstance->foot_height_c);

	FRotator foot_ik_rotator_target = CalculateFootRotation(HitResult.ImpactNormal);
	if (ShouldBreakIK(HitResult.Distance, AnimInstance->ik_break_standing_distance, AnimInstance->ik_break_crouching_distance, isCrouching)){
		foot_ik_target = FVector::ZeroVector;
		if (AnimInstance)
		{
			AnimInstance->ResetIK(foot_ikbone);

			//An example of the event usage
			AnimInstance->FeetLostIK();
		}
	}

	foot_ik_offset = FMath::VInterpTo(
		foot_ik_offset,
		foot_ik_target + lateral_offset,
		mesh->GetWorld()->GetDeltaSeconds(),
		SelectInterpSpeed(foot_ik_offset.Z, foot_ik_target.Z, AnimInstance->feet_interp_speed_up, AnimInstance->feet_interp_speed_down)
	);

	foot_ik_rotator = FMath::RInterpTo(
		foot_ik_rotator,
		foot_ik_rotator_target,
		mesh->GetWorld()->GetDeltaSeconds(),
		AnimInstance->feet_interp_speed_rotation
	);

}

float UThesisIKLibrary::CalculatePelvisAlpha(float left, float right)
{
	return (left + right) / 2.0f;
}

FVector UThesisIKLibrary::CalculatePelvisTarget(FVector left, FVector right)
{
	return (left.Z < right.Z) ? left : right;
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
	if (!mesh || !AnimInstance) return;


	UAnimInstance* AnimInst = Cast<UAnimInstance>(AnimInstance);
	if (!AnimInst) return;
	if (AnimInstance->ThesisDebugMode) {
		GEngine->AddOnScreenDebugMessage(14, 15.0f, FColor::Yellow, TEXT("===SetPelvisIKC Start===\nSetPelvisIKC -> (AnimInst) check: True"));
	}

	UWorld* World = mesh->GetWorld();
	if (!World) return;
	if (AnimInstance->ThesisDebugMode) {
		GEngine->AddOnScreenDebugMessage(15, 15.0f, FColor::Yellow, TEXT("SetPelvisIKC -> (World) check: True"));
	}

	pelvis_alpha = CalculatePelvisAlpha(
		AnimInst->GetCurveValue(AnimInstance->curve_left),
		AnimInst->GetCurveValue(AnimInstance->curve_right)
	);

	if (AnimInstance->force_curve_value) {
		pelvis_alpha = 1.0f;
	}
	if (pelvis_alpha <= 0) {
		pelvis_offset = FVector::ZeroVector;
		return;
	}
	if (AnimInstance->ThesisDebugMode) {
		GEngine->AddOnScreenDebugMessage(16, 15.0f, FColor::Yellow, TEXT("SetPelvisIKC -> (pelvis_alpha > 0) check: True\n===SetPelvisIKC End==="));
	}

	FVector pelvis_target = CalculatePelvisTarget(foot_ik_l_target, foot_ik_r_target);

	pelvis_offset = FMath::VInterpTo(
		pelvis_offset,
		pelvis_target,
		mesh->GetWorld()->GetDeltaSeconds(),
		SelectInterpSpeed(pelvis_offset.Z, pelvis_target.Z, AnimInstance->pelvis_interp_speed_up, AnimInstance->pelvis_interp_speed_down)
	);
}