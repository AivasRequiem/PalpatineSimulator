// Copyright(c) AivasGroup. All Rights Reserved.

#include "LaserProjectile.h"

#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PalpatineSimulator/GrippableObjects/PS_Lightsaber.h"

// Sets default values
ALaserProjectile::ALaserProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	Projectile = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile"));
	SetRootComponent(Projectile);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->SetUpdatedComponent(Projectile);
}

void ALaserProjectile::DeflectProjectile(const FHitResult& Hit)
{
	// Calculate the reflection direction
	FVector NewDirection = FMath::GetReflectionVector(ProjectileMovement->Velocity.GetSafeNormal(), Hit.Normal);
	
	float DotProduct = FVector::DotProduct(NewDirection, ProjectileMovement->Velocity.GetSafeNormal());
	float AngleInDegrees = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
	if (AngleInDegrees < 20.f && AngleInDegrees > -20.f)
	{
		constexpr float MaxAngleDeflection = 45.0f; // Maximum angle in degrees

		// Generate a random angle
		float RandomAnglePitch = FMath::RandRange(-MaxAngleDeflection, MaxAngleDeflection);
		float RandomAngleYaw = FMath::RandRange(-MaxAngleDeflection, MaxAngleDeflection);
		float RandomAngleRoll = FMath::RandRange(-MaxAngleDeflection, MaxAngleDeflection);
		FRotator RandomRotation = FRotator(RandomAnglePitch, RandomAngleYaw, RandomAngleRoll);

		// Rotate the base vector by the random angle
		NewDirection = RandomRotation.RotateVector(NewDirection) * -1;
	}
	
	UKismetSystemLibrary::DrawDebugArrow(this, Hit.Location, Hit.Location + NewDirection * 300, 10, FLinearColor::Black, 3);

	// Set the new direction for the projectile movement
	ProjectileMovement->Velocity = NewDirection * ProjectileMovement->InitialSpeed;
	SetActorRotation(ProjectileMovement->Velocity.GetSafeNormal().Rotation() + FRotator(-90, 0, 0));
}

void ALaserProjectile::OnHitEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                  FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->IsValidLowLevel())
	{
		if (OtherActor->IsA(APS_Lightsaber::StaticClass()))
		{
			if (!WasDeflected)
			{
				WasDeflected = true;
				DeflectProjectile(Hit);
			}
		}
	}
}

void ALaserProjectile::OnOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                      const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (OtherActor->IsA(APS_Lightsaber::StaticClass()))
		{
			return;
		}
	}


	UGameplayStatics::SpawnDecalAttached(WallBurnMaterial, FVector(5), OtherComp, NAME_None,
	                                     Projectile->GetSocketLocation(TopProjectileSocket),
	                                     ProjectileMovement->Velocity.GetSafeNormal().Rotation(),
	                                     EAttachLocation::KeepWorldPosition, 5.0f);
	Destroy();
}

// Called when the game starts or when spawned
void ALaserProjectile::BeginPlay()
{
	Super::BeginPlay();

	Projectile->OnComponentHit.AddDynamic(this, &ALaserProjectile::OnHitEvent);
	Projectile->OnComponentBeginOverlap.AddDynamic(this, &ALaserProjectile::OnOverlapEvent);
}
