// Copyright(c) AivasGroup. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GripMotionControllerComponent.h"
#include "Components/TimelineComponent.h"
#include "ForceHandComponent.generated.h"

/**
 * 
 */
UCLASS()
class PALPATINESIMULATOR_API UForceHandComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UForceHandComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(Category = "Force")
	void ForcePullActivate();
	UFUNCTION(Category = "Force")
	void ForcePullDeactivate();

	UPROPERTY()
	UGripMotionControllerComponent* HandController;

protected:
	FTimeline MoveTimeline;
	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* MovingCurve;

	UPROPERTY()
	UPrimitiveComponent* MoveTarget;

private:
	UFUNCTION()
	void MoveTimelineProgress(float Value);
	UFUNCTION()
	void MoveFinishedCallback();

	bool MoveTargetToHand() const;
	
	void TurnBackCollision();
	ECollisionEnabled::Type PreviousCollisionEnabled;
};
