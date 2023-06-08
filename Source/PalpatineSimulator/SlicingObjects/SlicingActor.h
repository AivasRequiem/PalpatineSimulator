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

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void MeshDissolveWithDelay(float DelayTime);
	
	void MeshSliced();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TArray<UStaticMesh*> RandomMeshesToCopy;
	UPROPERTY(EditAnywhere)
	bool ShouldChooseRandomMesh;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UProceduralMeshComponent* SlicingMesh;
	// Generate Convex Collision
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MeshToCopy;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<UProceduralMeshComponent*> SlicedMeshes;
};
