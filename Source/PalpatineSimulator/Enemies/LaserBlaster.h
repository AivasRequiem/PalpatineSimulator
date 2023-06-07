// Copyright(c) AivasGroup. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LaserProjectile.h"
#include "GameFramework/Actor.h"
#include "LaserBlaster.generated.h"

UCLASS()
class PALPATINESIMULATOR_API ALaserBlaster : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALaserBlaster();

	UFUNCTION(BlueprintCallable)
	void ShootProjectile();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* Blaster;

	UPROPERTY(EditAnywhere)
	bool AutoShooting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ProjectileClass = ALaserProjectile::StaticClass();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName MuzzleSocket = "Muzzle";

private:
	FTimerHandle ShootTimerHandle;
};
