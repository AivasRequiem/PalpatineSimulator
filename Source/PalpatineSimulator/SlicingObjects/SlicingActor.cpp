// Copyright(c) AivasGroup. All Rights Reserved.

#include "SlicingActor.h"
#include "ProceduralMeshComponent.h"
#include "PalpatineSimulator/Core/PS_GameState.h"

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
	MeshToCopy->SetupAttachment(SlicingMesh);
	MeshToCopy->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshToCopy->SetHiddenInGame(true);

	SlicedMeshes.AddUnique(SlicingMesh);
}

void ASlicingActor::BeginPlay()
{
	Super::BeginPlay();

	if (ShouldChooseRandomMesh)
	{
		const int32 RandomIndex = FMath::RandRange(0, RandomMeshesToCopy.Num() - 1);
		MeshToCopy->SetStaticMesh(RandomMeshesToCopy[RandomIndex]);
	}

	MeshToCopy->GetStaticMesh()->bAllowCPUAccess = true;
	UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(MeshToCopy, 0, SlicingMesh, true);
}

void ASlicingActor::MeshSliced()
{
	if (APS_GameState* GameState = GetWorld()->GetGameState<APS_GameState>())
	{
		GameState->IncrementObjectsSliced();
	}

	MeshDissolveWithDelay(3.0f);
}
