// Copyright(c)  AivasGroup. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Components/TimelineComponent.h"
#include "Grippables/GrippableActor.h"
#include "Grippables/HandSocketComponent.h"
#include "Sound/SoundCue.h"
#include "PalpatineSimulator/Interfaces/InteractableItem.h"
#include "PS_Lightsaber.generated.h"

UENUM()
enum class EBladeColor : uint8
{
	Red,
	Blue,
	Green,
	Purple,
	White
};

UCLASS()
class PALPATINESIMULATOR_API APS_Lightsaber : public AGrippableActor, public IInteractableItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APS_Lightsaber(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	bool IsTurnedOn();
	UFUNCTION(BlueprintCallable)
	void TurnOn();
	UFUNCTION(BlueprintCallable)
	void TurnOff();

	virtual void ActivateItem_Implementation() override;

	// Event triggered on the interfaced object when grip is released
	virtual void OnGripRelease_Implementation(UGripMotionControllerComponent* ReleasingController,
	                                          const FBPActorGripInformation& GripInformation,
	                                          bool bWasSocketed = false) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* Hilt;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* Blade;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAudioComponent* HummingAudio;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAudioComponent* ActionAudio;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grip")
	UHandSocketComponent* PrimaryGripSocket;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grip")
	UHandSocketComponent* SecondaryGripSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* BladeCrossSparks;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* WallBurnVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBladeColor BladeColor;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	FTimeline TurnOnTimeline;
	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* TurningOnCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundCue* ActivationSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundCue* DeactivationSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundCue* HummingSound;

	bool TurnedOn;

	// Slicing
	UFUNCTION(Category="Slicing")
	void OnBladeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                       int32 OtherBodyIndex);

	UPROPERTY(BlueprintReadOnly, Category="Slicing")
	bool IsSlicing = false;
	UPROPERTY(BlueprintReadOnly, Category="Slicing")
	FVector StartSlicePoint;

	UPROPERTY(BlueprintReadOnly)
	FName BottomBladeSocket = "Bottom";
	UPROPERTY(BlueprintReadOnly)
	FName TopBladeSocket = "Top";

private:
	UFUNCTION()
	void TurnOnTimelineProgress(float Value);
	UFUNCTION()
	void TimelineFinishedCallback();

	UPROPERTY(EditDefaultsOnly)
	TMap<EBladeColor, FLinearColor> PossibleBladeColors;
};
