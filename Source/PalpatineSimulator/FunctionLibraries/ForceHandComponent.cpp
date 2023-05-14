// Copyright(c) AivasGroup. All Rights Reserved.

#include "ForceHandComponent.h"
#include "Math/UnrealMathUtility.h"

UForceHandComponent::UForceHandComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UForceHandComponent::BeginPlay()
{
	Super::BeginPlay();

	if (MovingCurve != nullptr)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("MoveTimelineProgress"));
		MoveTimeline.AddInterpFloat(MovingCurve, TimelineProgress);
		MoveTimeline.SetLooping(false);
		FOnTimelineEventStatic TimelineFinishedCallback;
		TimelineFinishedCallback.BindUFunction(this, FName("MoveFinishedCallback"));
		MoveTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
	}
}

void UForceHandComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	MoveTimeline.TickTimeline(DeltaTime);
}

void UForceHandComponent::ForcePullActivate()
{
	FHitResult HitResult;
	const bool IsHit = GetWorld()->LineTraceSingleByChannel(HitResult,
	                                                        HandController->GetComponentLocation(),
	                                                        HandController->GetComponentLocation() + HandController->GetForwardVector() * 1500.0f,
	                                                        ECC_Visibility);

	if (IsHit)
	{
		if (HitResult.Component->IsAnySimulatingPhysics())
		{
			MoveTarget = HitResult.Component.Get();
			MoveTarget->SetSimulatePhysics(false);
			MoveTimeline.SetPlayRate(0.3f);
			MoveTimeline.PlayFromStart();
		}
	}
}

void UForceHandComponent::ForcePullDeactivate()
{
	MoveTimeline.Stop();

	if (MoveTarget->IsValidLowLevel())
	{
		MoveTarget->SetSimulatePhysics(true);
		MoveTarget = nullptr;
	}
}

void UForceHandComponent::MoveTimelineProgress(float Value)
{
	const FVector CurrentPosition = MoveTarget->GetComponentLocation();

	FVector FinalPosition;
	if (MoveTargetToHand())
	{
		FinalPosition = HandController->GetComponentLocation();
	}
	else
	{
		FinalPosition = HandController->GetComponentLocation() + HandController->GetForwardVector() * 200.0f;
	}

	MoveTarget->SetWorldLocation(FMath::Lerp(CurrentPosition, FinalPosition, Value), true);
}

void UForceHandComponent::MoveFinishedCallback()
{
	MoveTimeline.PlayFromStart();
}

bool UForceHandComponent::MoveTargetToHand() const
{
	if (MoveTarget->Implements<UVRGripInterface>())
	{
		return !IVRGripInterface::Execute_DenyGripping(MoveTarget, HandController);
	}

	if (MoveTarget->GetOwner()->Implements<UVRGripInterface>())
	{
		return !IVRGripInterface::Execute_DenyGripping(MoveTarget->GetOwner(), HandController);
	}

	return false;
}
