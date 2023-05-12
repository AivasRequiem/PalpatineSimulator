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

	UFUNCTION(BlueprintNativeEvent)
	void MeshSliced();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UProceduralMeshComponent* SlicingMesh;
	// Allow CPUAccess for Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MeshToCopy;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<UProceduralMeshComponent*> SlicedMeshes;
};
