// Copyright(c) AivasGroup. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "LaserProjectile.generated.h"

UCLASS()
class PALPATINESIMULATOR_API ALaserProjectile : public AActor
{
	GENERATED_BODY()

public:
	ALaserProjectile();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Projectile;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* ProjectileMovement;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnHitEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void OnOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void DeflectProjectile(const FHitResult& Hit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterial* WallBurnMaterial;

	bool WasDeflected;
	UPROPERTY(BlueprintReadOnly)
	FName TopProjectileSocket = "Top";
};
