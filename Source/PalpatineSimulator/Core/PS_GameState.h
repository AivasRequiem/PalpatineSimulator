// Copyright(c) AivasGroup. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "PS_GameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnoughLasersDeflected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnoughObjectsSliced);

UCLASS()
class PALPATINESIMULATOR_API APS_GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsEnoughLasersDeflected();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsEnoughObjectsSliced();

	UFUNCTION(BlueprintCallable)
	void IncrementLasersDeflected();
	UFUNCTION(BlueprintCallable)
	void IncrementObjectsSliced();
	
	UPROPERTY(BlueprintAssignable)
	FOnEnoughLasersDeflected OnEnoughLasersDeflected;
	UPROPERTY(BlueprintAssignable)
	FOnEnoughObjectsSliced OnEnoughObjectsSliced;

protected:
	virtual void BeginPlay() override;
	
private:
	uint32 LasersDeflected = 0;
	uint32 ObjectsSliced = 0;
	
	uint32 NeededLasersDeflected = 3;
	uint32 NeededObjectsSliced = 3;
};
