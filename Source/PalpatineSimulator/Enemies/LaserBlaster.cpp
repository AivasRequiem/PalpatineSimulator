// Copyright(c) AivasGroup. All Rights Reserved.

#include "LaserBlaster.h"

// Sets default values
ALaserBlaster::ALaserBlaster()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Blaster = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Blaster"));
	SetRootComponent(Blaster);
}

void ALaserBlaster::ShootProjectile()
{
	FVector MuzzleLocation = Blaster->GetSocketLocation(MuzzleSocket);
	FRotator MuzzleRotation = Blaster->GetSocketRotation(MuzzleSocket);
	FActorSpawnParameters ProjectileSpawnParams;
	ProjectileSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ALaserProjectile* Projectile = GetWorld()->SpawnActor<ALaserProjectile>(
		ProjectileClass, MuzzleLocation, MuzzleRotation, ProjectileSpawnParams);

	if (Projectile)
	{
		Projectile->ProjectileMovement->Velocity = GetActorForwardVector() * Projectile->ProjectileMovement->
			InitialSpeed;
		Projectile->ProjectileMovement->UpdateComponentVelocity();
	}
}

// Called when the game starts or when spawned
void ALaserBlaster::BeginPlay()
{
	Super::BeginPlay();

	if (AutoShooting)
	{
		GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &ALaserBlaster::ShootProjectile, 3.0f, true, 0);
	}
}
