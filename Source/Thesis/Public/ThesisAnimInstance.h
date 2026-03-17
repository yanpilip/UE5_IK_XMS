#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ThesisAnimInstance.generated.h"

class AThesisCharacter;
class UCharacterMovementComponent;

UCLASS()
class THESIS_API UThesisAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	void UpdateCharacterState();
	void UpdateIK(float DeltaSeconds);
	void ResetIK();
	void ExecuteDelegates();

public:


	UFUNCTION(BlueprintPure, Category = "Character")
	bool GetIsCrouchedState() const;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	ACharacter* Character = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	UCharacterMovementComponent* MovementComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	float GroundSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	float Direction = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	bool ShouldMove = false;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	bool IsFalling = false;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	bool IsCrouched = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Features")
	bool ShouldCrouch = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "IK")
	FVector FootLGoalPosition = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "IK")
	FVector FootRGoalPosition = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "IK")
	FVector PelvisGoalPosition = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "IK")
	FRotator FootLGoalRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "IK")
	FRotator FootRGoalRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "IK")
	float CrouchRootOffset = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "IK")
	float FootLIKAlpha = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "IK")
	float FootRIKAlpha = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK|Sockets")
	FName LeftFootSocketName = TEXT("foot_l_Socket");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK|Sockets")
	FName RightFootSocketName = TEXT("foot_r_Socket");

	UFUNCTION(BlueprintImplementableEvent, Category = "Features")
	void OnOneLegLostIK();
};