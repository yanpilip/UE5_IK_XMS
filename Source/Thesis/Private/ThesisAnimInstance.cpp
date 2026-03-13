#include "ThesisAnimInstance.h"
#include "ThesisCharacter.h"
#include "ThesisIKLibrary.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "KismetAnimationLibrary.h"

void UThesisAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<AThesisCharacter>(TryGetPawnOwner());
}

void UThesisAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!Character)
	{
		Character = Cast<AThesisCharacter>(TryGetPawnOwner());
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

	UpdateCharacterState();
	//if (!IsFalling) {
		//UpdateIK(DeltaSeconds);
	//}
	//else
	//{
		//ResetIK();
	//}
}

void UThesisAnimInstance::ResetIK()
{
	FootLGoalPosition = FVector::ZeroVector;
	FootRGoalPosition = FVector::ZeroVector;

	FootLGoalRotation = FRotator::ZeroRotator;
	FootRGoalRotation = FRotator::ZeroRotator;

	PelvisGoalPosition = FVector::ZeroVector;
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
		IsCrouched = MovementComponent->IsCrouching();
	}
	else
	{
		IsFalling = false;
		IsCrouched = false;
	}
}

void UThesisAnimInstance::UpdateIK(float DeltaSeconds)
{
	if (!Character)
	{
		return;
	}

	FVector RootLocation = Character->GetActorLocation();

	if (Character->GetCapsuleComponent())
	{
		RootLocation.Z -= Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}

	bool ShouldTraceL = true;
	bool ShouldTraceR = true;

	UThesisIKLibrary::FootPlacement(
		Character,
		FootLGoalPosition,
		FootRGoalPosition,
		RootLocation,
		ShouldTraceL,
		ShouldTraceR
	);

	FVector OutOffsetL = FVector::ZeroVector;
	FVector OutOffsetR = FVector::ZeroVector;
	FVector OutOffsetPel = FVector::ZeroVector;
	FRotator OutRotL = FRotator::ZeroRotator;
	FRotator OutRotR = FRotator::ZeroRotator;
	FHitResult OutLHit;
	FHitResult OutRHit;
	bool OutIfHit = false;
	bool OutShouldRagdoll = false;

	UThesisIKLibrary::ComputeFootOffsetAndRotation(
		Character,
		FootLGoalPosition,
		FootRGoalPosition,
		RootLocation,
		ShouldTraceL,
		ShouldTraceR,
		OutOffsetL,
		OutRotL,
		OutOffsetR,
		OutRotR,
		OutOffsetPel,
		OutLHit,
		OutRHit,
		OutIfHit,
		OutShouldRagdoll
	);

	if (OutShouldRagdoll)
	{
		PelvisGoalPosition = FVector::ZeroVector;
		FootLGoalRotation = FRotator::ZeroRotator;
		FootRGoalRotation = FRotator::ZeroRotator;
		return;
	}

	if (OutIfHit)
	{
		FVector TargetPelvis = PelvisGoalPosition;
		TargetPelvis.Z = OutOffsetPel.Z;

		PelvisGoalPosition = FMath::VInterpTo(
			PelvisGoalPosition,
			TargetPelvis,
			DeltaSeconds,
			PelvisInterpSpeed
		);

		FootLGoalPosition = OutOffsetL;
		FootRGoalPosition = OutOffsetR;
		FootLGoalRotation = OutRotL;
		FootRGoalRotation = OutRotR;
	}
}