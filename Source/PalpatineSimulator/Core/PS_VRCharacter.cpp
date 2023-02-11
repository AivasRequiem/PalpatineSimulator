// Copyright(c)  AivasGroup. All Rights Reserved.


#include "PS_VRCharacter.h"


// Sets default values
APS_VRCharacter::APS_VRCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APS_VRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APS_VRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APS_VRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

