// Copyright(c) AivasGroup. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "PS_GameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnoughLasersDeflected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnoughObjectsSliced);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVOEndPlaying);

UCLASS()
class PALPATINESIMULATOR_API APS_GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	APS_GameState();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsEnoughLasersDeflected();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsEnoughObjectsSliced();

	UFUNCTION(BlueprintCallable)
	void IncrementLasersDeflected();
	UFUNCTION(BlueprintCallable)
	void IncrementObjectsSliced();

	UFUNCTION(BlueprintCallable, Category="Audio")
	void PlaySoundInstantly(USoundCue* SoundToPlay);
	UFUNCTION(BlueprintCallable, Category="Audio")
	void AddSoundToPlayQueue(USoundCue* SoundToPlay);
	UFUNCTION(Category="Audio")
	void OnAudioEndPlaying();
	
	UPROPERTY(BlueprintAssignable)
	FOnEnoughLasersDeflected OnEnoughLasersDeflected;
	UPROPERTY(BlueprintAssignable)
	FOnEnoughObjectsSliced OnEnoughObjectsSliced;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	UAudioComponent* AudioComponent;
	UPROPERTY(BlueprintAssignable, Category="Audio")
	FOnVOEndPlaying OnVoEndPlaying;

protected:
	virtual void BeginPlay() override;
	
private:
	uint32 LasersDeflected = 0;
	uint32 ObjectsSliced = 0;
	
	uint32 NeededLasersDeflected = 3;
	uint32 NeededObjectsSliced = 5;

	TQueue<USoundCue*> PlayQueue;
};
