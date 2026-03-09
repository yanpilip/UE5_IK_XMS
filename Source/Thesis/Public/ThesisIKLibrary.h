// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ThesisIKLibrary.generated.h"

/**
 * 
 */
UCLASS()
class THESIS_API UThesisIKLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	

public:
	UFUNCTION(BlueprintCallable, Category = "Thesis|IK", meta = (WorldContext = "WorldContextObject"))
	static void ComputeFootOffsetAndRotation(
		const UObject* WorldContextObject,
		const FVector& footLocation,
		const FVector& rootLocation,
		FVector& OutOffset,
		FRotator& OutRot
	);
	UFUNCTION(BlueprintCallable, Category = "Thesis|IK", meta = (WorldContext = "WorldContextObject"))
	static void ComputeFootOffsetAndRotation2(
		const UObject* WorldContextObject,
		const FVector& LfootLocation,
		const FVector& RfootLocation,
		const FVector& rootLocation,
		const float CapsuleOffset,
		FVector& OutOffsetL,
		FRotator& OutRotL,
		FVector& OutOffsetR,
		FRotator& OutRotR,
		FVector& OutOffsetPel,
		FHitResult& OutLHit,
		FHitResult& OutRHit,
		bool& OutIfHit
	);
	UFUNCTION(BlueprintCallable, Category = "Thesis|IK", meta = (WorldContext = "WorldContextObject"))
	static void ComputeFootOffsetAndRotation3(
		const UObject* WorldContextObject,
		const FVector& footLocation,
		const FVector& rootLocation,
		FVector& OutOffset,
		FRotator& OutRot
	);
};
