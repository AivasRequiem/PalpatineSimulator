// Copyright(c)  AivasGroup. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Grippables/GrippableActor.h"
#include "PS_Lightsaber.generated.h"

UCLASS()
class PALPATINESIMULATOR_API APS_Lightsaber : public AGrippableActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APS_Lightsaber(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* Hilt;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* Blade;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
