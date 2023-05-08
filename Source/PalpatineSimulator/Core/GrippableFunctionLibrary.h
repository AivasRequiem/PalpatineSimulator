// Copyright(c)  AivasGroup. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GripMotionControllerComponent.h"
#include "Components/SphereComponent.h"
#include "GrippableFunctionLibrary.generated.h"

struct FOverlappingInfo
{
	UObject* ObjectToGrip = nullptr;
	FTransform ObjectTransform;
	FName Socket;
	FVector ImpactLocation;
};

/**
 * 
 */
UCLASS()
class PALPATINESIMULATOR_API UGrippableFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void DropObject(UGripMotionControllerComponent* CallingController);
	

	static void GetNearestOverlappingObject(const USphereComponent* GrabSphere, FOverlappingInfo& OverlappingInfo);
	static bool CanGripObject(UObject* ObjectToGrip);
	static bool CanSecondaryGripObject(UObject* ObjectToGrip, ESecondaryGripType& SecondaryGripType);
	static FTransform GetBoneTransform(UObject* Object, FName Socket);
	static bool GripObject(UGripMotionControllerComponent* CallingController, UGripMotionControllerComponent* OtherController, USphereComponent* GrabSphere);
};
