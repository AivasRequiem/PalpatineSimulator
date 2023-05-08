// Copyright(c)  AivasGroup. All Rights Reserved.

#include "PS_Lightsaber.h"

// Sets default values
APS_Lightsaber::APS_Lightsaber(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Hilt = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hilt"));
	SetRootComponent(Hilt);

	Blade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Blade"));
	Blade->SetupAttachment(Hilt);

	PrimaryGripSocket = CreateDefaultSubobject<UHandSocketComponent>(TEXT("PrimaryGripSocket"));
	PrimaryGripSocket->SetupAttachment(Hilt);

	SecondaryGripSocket = CreateDefaultSubobject<UHandSocketComponent>(TEXT("SecondaryGripSocket"));
	SecondaryGripSocket->SetupAttachment(Hilt);
}

// Called when the game starts or when spawned
void APS_Lightsaber::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APS_Lightsaber::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
