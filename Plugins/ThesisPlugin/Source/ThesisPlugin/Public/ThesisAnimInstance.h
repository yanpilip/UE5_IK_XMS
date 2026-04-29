#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
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
	void ResetIK(FName legname);

	UFUNCTION(BlueprintPure, Category = "Thesis_Plugin_State")
	bool GetIsCrouchedState() const;

	UPROPERTY(BlueprintReadWrite, Category = "Thesis_Plugin_State")
	ACharacter* ThesisCharacter = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Thesis_Plugin_State")
	UCharacterMovementComponent* ThesisMovementComponent = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Thesis_Plugin_State")
	FVector ThesisVelocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Thesis_Plugin_State")
	float ThesisGroundSpeed = 0.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Thesis_Plugin_State")
	float ThesisDirection = 0.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Thesis_Plugin_State")
	bool ThesisShouldMove = false;

	UPROPERTY(BlueprintReadWrite, Category = "Thesis_Plugin_State")
	bool ThesisIsFalling = false;

	UPROPERTY(BlueprintReadWrite, Category = "Thesis_Plugin_State")
	bool ThesisIsCrouched = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Features")
	bool ThesisShouldCrouch = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Features")
	bool EnableFootIK = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Features")
	bool ThesisDebugMode = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Features")
	bool force_curve_value = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|FootIK|Offsets")
	FVector foot_ik_l_offset_c = FVector(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|FootIK|Offsets")
	FVector foot_ik_r_offset_c = FVector(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|FootIK|Targets")
	FVector foot_ik_l_target_c = FVector(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|FootIK|Targets")
	FVector foot_ik_r_target_c = FVector(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|FootIK|Rotation")
	FRotator foot_ik_l_rotation_c = FRotator(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|FootIK|Rotation")
	FRotator foot_ik_r_rotation_c = FRotator(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|FootIK|Offsets")
	FVector pelvis_offset_c = FVector(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|FootIK|Offsets")
	float ik_stance_width_offset = float(0.0f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|FootIK|Offsets")
	FVector ik_left_knee_bending_direction = FVector(20.0f, 30.0f, 0.0f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|FootIK|Offsets")
	FVector ik_right_knee_bending_direction = FVector(-20.0f, -30.0f, 0.0f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|FootIK|Other Variables")
	float pelvis_alpha_c = float(1.0f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|FootIK|Other Variables")
	float foot_height_c = float(13.5f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|FootIK|Other Variables")
	float ik_break_standing_distance = float(100.0f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|FootIK|Other Variables")
	float ik_break_crouching_distance = float(80.0f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|Interpolations|Feet")
	float feet_interp_speed_up = float(30.0f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|Interpolations|Feet")
	float feet_interp_speed_down = float(85.0f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|Interpolations|Feet")
	float feet_interp_speed_rotation = float(10.0f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|Interpolations|Pelvis")
	float pelvis_interp_speed_up = float(10.0f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|Interpolations|Pelvis")
	float pelvis_interp_speed_down = float(15.0f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|Interpolations")
	float reset_ik_speed = float(10.0f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|Trace")
	FVector foot_trace_above = FVector(0.0f, 0.0f, 50.0f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|Trace")
	FVector foot_trace_below = FVector(0.0f, 0.0f, 60.0f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|Names|IK Curves")
	FName curve_left = FName("Enable_FootIK_L");

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|Names|IK Curves")
	FName curve_right = FName("Enable_FootIK_R");

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|Names|Bones|IK Bones")
	FName foot_ik_l_name = FName("ik_foot_l");

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|Names|Bones|IK Bone")
	FName foot_ik_r_name = FName("ik_foot_r");

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Thesis|Variables|Names|Bones")
	FName root_bone_name = FName("root");

	UPROPERTY(BlueprintReadWrite, Category = "Thesis|Mesh")
	USkeletalMeshComponent* ThesisMesh = nullptr;

	UFUNCTION(BlueprintImplementableEvent, Category = "Features")
	void FeetLostIK();
};