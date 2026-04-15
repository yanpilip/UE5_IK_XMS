#include "ThesisAnimInstance.h"
#include "ThesisIKLibrary.h"
#include "ThesisPlugin.h"
#include "CoreMinimal.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

void UThesisAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ACharacter>(TryGetPawnOwner());
}

bool UThesisAnimInstance::GetIsCrouchedState() const {
	return ((MovementComponent && ShouldCrouch) ? MovementComponent->IsCrouching() : false);
}

void UThesisAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!Character)
	{
		Character = Cast<ACharacter>(TryGetPawnOwner());
	}

	if (!Character)
	{
		return;
	}

	MovementComponent = Character->GetCharacterMovement();
	if (!MovementComponent)
	{
		return;
	}
	if (EnableFootIK && (GetSkelMeshComponent()->GetAnimInstance() == this)) {
		if (!IsFalling) {
			UpdateFootIKC(DeltaSeconds);
		}
		else
		{
			UThesisIKLibrary::SetPelvisIKC(Character->GetMesh(), this, FVector(0, 0, 0), FVector(0, 0, 0), pelvis_alpha_c, pelvis_offset_c);
			ResetIK(foot_ik_l_name);
			ResetIK(foot_ik_r_name);
		}
	}
	UpdateCharacterState();
}

void UThesisAnimInstance::UpdateCharacterState()
{
	Velocity = Character->GetVelocity();

	GroundSpeed = Velocity.Size2D();


	if (MovementComponent->bOrientRotationToMovement) {
		Direction = FMath::Clamp(UKismetAnimationLibrary::CalculateDirection(Velocity, Character->GetActorRotation()), -45.0f, 45.0f);
	}
	else {
		Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Character->GetActorRotation());
	}

	ShouldMove = GroundSpeed > 0.01f && !(MovementComponent->GetCurrentAcceleration().Equals(FVector::ZeroVector, 0.0f));
	if (Character->GetCharacterMovement())
	{
		IsFalling = MovementComponent->IsFalling();
		IsCrouched = GetIsCrouchedState();
	}
	else
	{
		IsFalling = false;
		IsCrouched = false;
	}
}

void UThesisAnimInstance::UpdateFootIKC(float DeltaSeconds)
{
	if (!Character)
	{
		return;
	}
	UThesisIKLibrary::SetFootIKC(Character->GetMesh(), Character, this, IsCrouched, curve_left, foot_ik_l_name, root_bone_name, foot_ik_l_offset_c, foot_ik_l_target_c, foot_ik_l_rotation_c);
	UThesisIKLibrary::SetFootIKC(Character->GetMesh(), Character, this, IsCrouched, curve_right, foot_ik_r_name, root_bone_name, foot_ik_r_offset_c, foot_ik_r_target_c, foot_ik_r_rotation_c);

	UThesisIKLibrary::SetPelvisIKC(Character->GetMesh(), this, foot_ik_l_target_c, foot_ik_r_target_c, pelvis_alpha_c, pelvis_offset_c);

}

void UThesisAnimInstance::ResetIK(FName legname) {
	if (legname == foot_ik_l_name) {
		foot_ik_l_offset_c = FMath::VInterpTo(
			foot_ik_l_offset_c,
			FVector(0, 0, 0),
			GetWorld()->GetDeltaSeconds(),
			reset_ik_speed
		);
		foot_ik_r_offset_c = FMath::VInterpTo(
			foot_ik_r_offset_c,
			FVector(0, 0, 0),
			GetWorld()->GetDeltaSeconds(),
			reset_ik_speed
		);
	}
	else {
		foot_ik_l_rotation_c = FMath::RInterpTo(
			foot_ik_l_rotation_c,
			FRotator(0, 0, 0),
			GetWorld()->GetDeltaSeconds(),
			reset_ik_speed
		);
		foot_ik_r_rotation_c = FMath::RInterpTo(
			foot_ik_r_rotation_c,
			FRotator(0, 0, 0),
			GetWorld()->GetDeltaSeconds(),
			reset_ik_speed
		);
	}
}