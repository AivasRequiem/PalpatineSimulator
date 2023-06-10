// Copyright(c) AivasGroup. All Rights Reserved.

#include "PS_GameState.h"

APS_GameState::APS_GameState()
{
	AudioComponent = CreateDefaultSubobject<UAudioComponent>("Audio Component");
	AudioComponent->OnAudioFinished.AddDynamic(this, &APS_GameState::OnAudioEndPlaying);
}

bool APS_GameState::IsEnoughLasersDeflected()
{
	return LasersDeflected >= NeededLasersDeflected;
}

bool APS_GameState::IsEnoughObjectsSliced()
{
	return ObjectsSliced >= NeededObjectsSliced;
}

void APS_GameState::IncrementLasersDeflected()
{
	LasersDeflected++;

	if (IsEnoughLasersDeflected())
		OnEnoughLasersDeflected.Broadcast();
}

void APS_GameState::IncrementObjectsSliced()
{
	ObjectsSliced++;

	if (IsEnoughObjectsSliced())
		OnEnoughObjectsSliced.Broadcast();
}

void APS_GameState::PlaySoundInstantly(USoundCue* SoundToPlay)
{
	if (SoundToPlay)
	{
		PlayQueue.Enqueue(SoundToPlay);
		AudioComponent->SetSound(SoundToPlay);
		AudioComponent->Play();	
	}
}

void APS_GameState::AddSoundToPlayQueue(USoundCue* SoundToPlay)
{
	if (PlayQueue.IsEmpty())
		PlaySoundInstantly(SoundToPlay);
	else
		PlayQueue.Enqueue(SoundToPlay);
}

void APS_GameState::OnAudioEndPlaying()
{
	OnVoEndPlaying.Broadcast();

	USoundCue* Sound;
	PlayQueue.Dequeue(Sound);
	if (!PlayQueue.IsEmpty())
	{
		PlayQueue.Peek(Sound);
		AudioComponent->SetSound(Sound);
		AudioComponent->Play();
	}
}

void APS_GameState::BeginPlay()
{
	Super::BeginPlay();
}

