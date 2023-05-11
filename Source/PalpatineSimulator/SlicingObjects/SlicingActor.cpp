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

	MeshToCopy = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshToCopy"));
	MeshToCopy->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshToCopy->SetHiddenInGame(true);
	MeshToCopy->SetVisibility(false);

	//UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(MeshToCopy, 0, SlicingMesh, true);
}

// Called when the game starts or when spawned
void ASlicingActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASlicingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

