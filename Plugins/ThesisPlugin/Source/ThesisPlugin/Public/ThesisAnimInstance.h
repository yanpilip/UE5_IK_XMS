#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ThesisAnimInstance.generated.h"

class AThesisCharacter;
class UCharacterMovementComponent;

UCLASS()
class THESISPLUGIN_API UThesisAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	void UpdateCharacterState();
	void UpdateFootIKC(float DeltaSeconds);

public:
	void ResetIK();

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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Features")
	bool ShouldCrouch = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables")
	FVector foot_ik_l_offset_c = FVector(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables")
	FVector foot_ik_r_offset_c = FVector(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables")
	FVector foot_ik_l_target_c = FVector(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables")
	FVector foot_ik_r_target_c = FVector(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables")
	FRotator foot_ik_l_rotation_c = FRotator(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables")
	FRotator foot_ik_r_rotation_c = FRotator(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables")
	FVector pelvis_offset_c = FVector(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables")
	float pelvis_alpha_c = float(1.0f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables")
	float foot_height_c = float(13.5f);

	UFUNCTION(BlueprintImplementableEvent, Category = "Features")
	void FeetLostIK();
};