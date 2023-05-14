// Copyright(c) AivasGroup. All Rights Reserved.


#include "SlicingActor.h"

// Sets default values
ASlicingActor::ASlicingActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SlicingMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("SlicingMesh"));
	SetRootComponent(SlicingMesh);
	SlicingMesh->bUseComplexAsSimpleCollision = false;
	SlicingMesh->SetSimulatePhysics(true);
	SlicingMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);

	MeshToCopy = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshToCopy"));
	MeshToCopy->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshToCopy->SetHiddenInGame(true);
	MeshToCopy->SetVisibility(false);

	SlicedMeshes.AddUnique(SlicingMesh);
}

void ASlicingActor::MeshSliced_Implementation()
{
}
