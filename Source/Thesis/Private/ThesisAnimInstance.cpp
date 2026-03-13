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
	if (!IsFalling) {
		UpdateIK(DeltaSeconds);
	}
	else
	{
		ResetIK();
	}
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
	GEngine->AddOnScreenDebugMessage(1, 10, FColor::Yellow, FString::Printf(TEXT("%d"), ShouldMove));
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

	FVector LFootLocation = FVector::ZeroVector;
	FVector RFootLocation = FVector::ZeroVector;

	if (Character->GetMesh())
	{
		LFootLocation = Character->GetMesh()->GetSocketLocation(TEXT("foot_l_Socket"));
		RFootLocation = Character->GetMesh()->GetSocketLocation(TEXT("foot_r_Socket"));
	}
	else {
		return;
	}

	UThesisIKLibrary::FootPlacement(
		Character,
		LFootLocation,
		RFootLocation,
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
		LFootLocation,
		RFootLocation,
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

		//TODO Ragdoll
	}

	if (OutIfHit)
	{
		FVector TargetPelvis = PelvisGoalPosition;
		TargetPelvis.Z = OutOffsetPel.Z;

		PelvisGoalPosition = FMath::VInterpTo(
			PelvisGoalPosition,
			TargetPelvis,
			DeltaSeconds,
			15.0f
		);

		FVector TargetLFPos = FootLGoalPosition;
		TargetLFPos.Z = OutOffsetL.Z;

		FootLGoalPosition = FMath::VInterpTo(
			FootLGoalPosition,
			TargetLFPos,
			DeltaSeconds,
			15.0f
		);

		FVector TargetRFPos = FootRGoalPosition;
		TargetRFPos.Z = OutOffsetR.Z;

		FootRGoalPosition = FMath::VInterpTo(
			FootRGoalPosition,
			TargetRFPos,
			DeltaSeconds,
			15.0f
		);

		FRotator TargetLFRot = OutRotL;

		FootLGoalRotation = FMath::RInterpTo(
			FootLGoalRotation,
			TargetLFRot,
			DeltaSeconds,
			15.0f
		);

		FRotator TargetRFRot = OutRotR;

		FootRGoalRotation = FMath::RInterpTo(
			FootRGoalRotation,
			TargetRFRot,
			DeltaSeconds,
			15.0f
		);
	}
	else
	{
		ResetIK();
	}
}