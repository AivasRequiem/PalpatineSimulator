// Copyright(c)  AivasGroup. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VRCharacter.h"
#include "PS_VRCharacter.generated.h"

UCLASS()
class PALPATINESIMULATOR_API APS_VRCharacter : public AVRCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APS_VRCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
