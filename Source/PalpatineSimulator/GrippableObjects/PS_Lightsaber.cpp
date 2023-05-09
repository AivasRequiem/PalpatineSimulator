// Copyright(c)  AivasGroup. All Rights Reserved.

#include "PS_Lightsaber.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
APS_Lightsaber::APS_Lightsaber(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Hilt = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hilt"));
	SetRootComponent(Hilt);

	Blade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Blade"));
	Blade->SetupAttachment(Hilt);

	PrimaryGripSocket = CreateDefaultSubobject<UHandSocketComponent>(TEXT("PrimaryGripSocket"));
	PrimaryGripSocket->SetupAttachment(Hilt);

	SecondaryGripSocket = CreateDefaultSubobject<UHandSocketComponent>(TEXT("SecondaryGripSocket"));
	SecondaryGripSocket->SetupAttachment(Hilt);

	HummingAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("HummingAudio"));
	HummingAudio->SetSound(HummingSound);
	HummingAudio->SetAutoActivate(false);
	HummingAudio->SetVolumeMultiplier(0.0f);
	
	ActionAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("ActionAudio"));
	ActionAudio->SetAutoActivate(false);

	BladeCrossSparks = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BladeCrossSparks"));
	BladeCrossSparks->SetupAttachment(Blade);
	BladeCrossSparks->SetAutoActivate(false);
}

bool APS_Lightsaber::IsTurnedOn()
{
	return TurnedOn;
}

void APS_Lightsaber::TurnOn()
{
	if (!TurnedOn)
	{
		TurnedOn = true;
		TurnOnTimeline.Play();

		if (ActivationSound)
		{
			ActionAudio->SetSound(ActivationSound);
			ActionAudio->Play();
		}
		HummingAudio->Activate();
	}
}

void APS_Lightsaber::TurnOff()
{
	if (TurnedOn)
	{
		TurnedOn = false;
		TurnOnTimeline.Reverse();

		if (DeactivationSound)
		{
			ActionAudio->SetSound(DeactivationSound);
			ActionAudio->Play();
		}

		BladeCrossSparks->Deactivate();
	}
}

void APS_Lightsaber::ActivateItem_Implementation()
{
	IInteractableItem::ActivateItem_Implementation();
	
	if (TurnedOn)
	{
		TurnOff();
	}
	else
	{
		TurnOn();
	}
}

void APS_Lightsaber::OnGripRelease_Implementation(UGripMotionControllerComponent* ReleasingController,
                                                  const FBPActorGripInformation& GripInformation, bool bWasSocketed)
{
	Super::OnGripRelease_Implementation(ReleasingController, GripInformation, bWasSocketed);

	TurnOff();
}

void APS_Lightsaber::BeginPlay()
{
	Super::BeginPlay();

	// Setup Blade Color
	const FLinearColor LinearColor = *PossibleBladeColors.Find(BladeColor);
	FVector Vector;
	Vector.X = LinearColor.R;
	Vector.Y = LinearColor.G;
	Vector.Z = LinearColor.B;
	Blade->SetVectorParameterValueOnMaterials("Color", Vector);

	// Setup Color for BladeCrossSparks
	if (BladeCrossSparks)
	{
		BladeCrossSparks->SetColorParameter(FName("SwordColor"), *PossibleBladeColors.Find(BladeColor));
	}

	if (TurningOnCurve != nullptr)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("TurnOnTimelineProgress"));
		TurnOnTimeline.AddInterpFloat(TurningOnCurve, TimelineProgress);
		TurnOnTimeline.SetLooping(false);
		FOnTimelineEventStatic TimelineFinishedCallback;
		TimelineFinishedCallback.BindUFunction(this, FName("TimelineFinishedCallback"));
		TurnOnTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "APixoVRPlayerController::BeginPlay Setup ChangingFadeOpacityCurve");
	}
}

void APS_Lightsaber::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TurnOnTimeline.TickTimeline(DeltaTime);

	if (TurnedOn)
	{
		FHitResult HitResult;
		const bool IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, Blade->GetSocketLocation(BottomBladeSocket), Blade->GetSocketLocation(TopBladeSocket), ECC_Visibility);

		if (IsHit)
		{
			BladeCrossSparks->SetWorldLocation(HitResult.Location);
			BladeCrossSparks->SetWorldScale3D(FVector(UKismetMathLibrary::RandomFloatInRange(0.5, 0.9)));
			BladeCrossSparks->Activate();
		}
		else if (BladeCrossSparks->IsActive())
		{
			BladeCrossSparks->Deactivate();
		}
	}
}

void APS_Lightsaber::TurnOnTimelineProgress(float Value)
{
	Blade->SetRelativeScale3D(FVector(Blade->GetRelativeScale3D().X, Blade->GetRelativeScale3D().Y, Value));

	HummingAudio->SetVolumeMultiplier(TurnOnTimeline.GetPlaybackPosition() / TurnOnTimeline.GetTimelineLength());
}

void APS_Lightsaber::TimelineFinishedCallback()
{
	if (TurnOnTimeline.GetPlaybackPosition() == 0.0f)
	{
		HummingAudio->Deactivate();
	}
}
