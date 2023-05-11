// Copyright(c) AivasGroup. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KismetProceduralMeshLibrary.h"
#include "SlicingActor.generated.h"

UCLASS()
class PALPATINESIMULATOR_API ASlicingActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASlicingActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UProceduralMeshComponent* SlicingMesh;
	// Allow CPUAccess for Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MeshToCopy;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
