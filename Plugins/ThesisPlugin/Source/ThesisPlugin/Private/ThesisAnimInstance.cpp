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

	ThesisCharacter = Cast<ACharacter>(TryGetPawnOwner());
}

bool UThesisAnimInstance::GetIsCrouchedState() const {
	return ((ThesisMovementComponent && ThesisShouldCrouch) ? ThesisMovementComponent->IsCrouching() : false);
}

void UThesisAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* OwningPawn = TryGetPawnOwner();
	if (!OwningPawn) return;
	if (ThesisDebugMode) {
		GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Yellow, TEXT("===ThesisAnimInstance Start===\nThesisAnimInstance -> (OwningPawn) check: True"));
	}

	if (!ThesisCharacter)
	{
		ThesisCharacter = Cast<ACharacter>(OwningPawn);
	}
	if (!ThesisCharacter) return;
	if (ThesisDebugMode) {
		GEngine->AddOnScreenDebugMessage(2, 15.0f, FColor::Yellow, TEXT("ThesisAnimInstance -> (ThesisCharacter) check: True"));
	}

	if (!ThesisMovementComponent)
	{
		ThesisMovementComponent = ThesisCharacter->GetCharacterMovement();
	}
	if (!ThesisMovementComponent) return;
	if (ThesisDebugMode) {
		GEngine->AddOnScreenDebugMessage(3, 15.0f, FColor::Yellow, TEXT("ThesisAnimInstance -> (ThesisMovementComponent) check: True"));
	}


	ThesisMesh = GetSkelMeshComponent();
	if (ThesisDebugMode && ThesisMesh) {
		GEngine->AddOnScreenDebugMessage(4, 15.0f, FColor::Yellow, TEXT("ThesisAnimInstance -> (ThesisMesh) check: True"));
	}
	if (!ThesisMesh || ThesisMesh->GetOwner() != GetOwningActor()) return;
	if (ThesisDebugMode) {
		GEngine->AddOnScreenDebugMessage(5, 15.0f, FColor::Yellow, TEXT("ThesisAnimInstance -> (ThesisMesh->GetOwner() != GetOwningActor()) check: True\n===ThesisAnimInstance End==="));
	}


	UpdateCharacterState();
	if (EnableFootIK && (ThesisMesh->GetAnimInstance() == this)) {
		if (!ThesisIsFalling) {
			UpdateFootIKC(DeltaSeconds);
		}
		else
		{
			UThesisIKLibrary::SetPelvisIKC(ThesisMesh, this, FVector(0, 0, 0), FVector(0, 0, 0), pelvis_alpha_c, pelvis_offset_c);
			ResetIK(foot_ik_l_name);
			ResetIK(foot_ik_r_name);
		}
	}
}

void UThesisAnimInstance::UpdateCharacterState()
{
	ThesisVelocity = ThesisCharacter->GetVelocity();

	ThesisGroundSpeed = ThesisVelocity.Size2D();


	if (ThesisMovementComponent->bOrientRotationToMovement) {
		ThesisDirection = FMath::Clamp(UKismetAnimationLibrary::CalculateDirection(ThesisVelocity, ThesisCharacter->GetActorRotation()), -45.0f, 45.0f);
	}
	else {
		ThesisDirection = UKismetAnimationLibrary::CalculateDirection(ThesisVelocity, ThesisCharacter->GetActorRotation());
	}

	ThesisShouldMove = ThesisGroundSpeed > 0.01f && !(ThesisMovementComponent->GetCurrentAcceleration().Equals(FVector::ZeroVector, 0.0f));
	if (ThesisCharacter->GetCharacterMovement())
	{
		ThesisIsFalling = ThesisMovementComponent->IsFalling();
		ThesisIsCrouched = GetIsCrouchedState();
	}
	else
	{
		ThesisIsFalling = false;
		ThesisIsCrouched = false;
	}
}

void UThesisAnimInstance::UpdateFootIKC(float DeltaSeconds)
{
	if (!ThesisCharacter)
	{
		return;
	}
	if (ThesisDebugMode) {
		GEngine->AddOnScreenDebugMessage(6, 15.0f, FColor::Yellow, TEXT("ThesisAnimInstance -> UpdateFootIKC -> (ThesisCharacter) check: True"));
	}
	UThesisIKLibrary::SetFootIKC(ThesisMesh, ThesisCharacter, this, ThesisIsCrouched, curve_left, foot_ik_l_name, root_bone_name, foot_ik_l_offset_c, foot_ik_l_target_c, foot_ik_l_rotation_c);
	UThesisIKLibrary::SetFootIKC(ThesisMesh, ThesisCharacter, this, ThesisIsCrouched, curve_right, foot_ik_r_name, root_bone_name, foot_ik_r_offset_c, foot_ik_r_target_c, foot_ik_r_rotation_c);

	UThesisIKLibrary::SetPelvisIKC(ThesisMesh, this, foot_ik_l_target_c, foot_ik_r_target_c, pelvis_alpha_c, pelvis_offset_c);

}

void UThesisAnimInstance::ResetIK(FName legname) {
	if (legname == foot_ik_l_name) {
		foot_ik_l_offset_c = FMath::VInterpTo(
			foot_ik_l_offset_c,
			FVector(0, 0, 0),
			GetWorld()->GetDeltaSeconds(),
			reset_ik_speed
		);
		foot_ik_l_rotation_c = FMath::RInterpTo(
			foot_ik_l_rotation_c,
			FRotator(0, 0, 0),
			GetWorld()->GetDeltaSeconds(),
			reset_ik_speed
		);
	}
	else if (legname == foot_ik_r_name) {
		foot_ik_r_offset_c = FMath::VInterpTo(
			foot_ik_r_offset_c,
			FVector(0, 0, 0),
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