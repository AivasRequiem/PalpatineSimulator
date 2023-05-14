// Copyright(c) AivasGroup. All Rights Reserved.

#include "ForceHandComponent.h"
#include "Kismet/KismetSystemLibrary.h"
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
	TArray<FHitResult> HitResults;
	TArray<AActor*> ActorsToIgnore;
	const bool IsHit = UKismetSystemLibrary::BoxTraceMulti(this, HandController->GetPivotLocation(),
	                                                        HandController->GetPivotLocation() + HandController->CustomPivotComponent->GetForwardVector() * 1000.0f, FVector(10, 10, 10),
	                                                        HandController->CustomPivotComponent->GetComponentRotation(),
	                                                        UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), true,
	                                                        ActorsToIgnore, EDrawDebugTrace::None, HitResults,
	                                                        true);

	if (IsHit)
	{
		for (FHitResult HitResult : HitResults)
		{
			if (HitResult.Component->IsAnySimulatingPhysics())
			{
				MoveTarget = HitResult.Component.Get();
				MoveTarget->SetSimulatePhysics(false);
				MoveTimeline.SetPlayRate(0.3f);
				MoveTimeline.PlayFromStart();
				break;
			}
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
		FinalPosition = HandController->CustomPivotComponent->GetComponentLocation();
	}
	else
	{
		FinalPosition = HandController->CustomPivotComponent->GetComponentLocation() + HandController->CustomPivotComponent->GetForwardVector() * 100.0f;
	}

	const FVector DesiredLocation = FMath::Lerp(CurrentPosition, FinalPosition, Value);
	MoveTarget->SetWorldLocation(DesiredLocation, true);
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
