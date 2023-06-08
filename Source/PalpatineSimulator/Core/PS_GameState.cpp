// Copyright(c) AivasGroup. All Rights Reserved.

#include "PS_GameState.h"

bool APS_GameState::IsEnoughLasersDeflected()
{
	return LasersDeflected == NeededLasersDeflected;
}

bool APS_GameState::IsEnoughObjectsSliced()
{
	return ObjectsSliced == NeededObjectsSliced;
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

void APS_GameState::BeginPlay()
{
	Super::BeginPlay();
}

