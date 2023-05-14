// Copyright(c)  AivasGroup. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VRCharacter.h"
#include "PS_InputConfigData.h"
#include "Components/SphereComponent.h"
#include "EnhancedInput/Public/InputMappingContext.h"
#include "PalpatineSimulator/CharacterTools/PS_Teleporter.h"
#include "PalpatineSimulator/FunctionLibraries/ForceHandComponent.h"
#include "PS_VRCharacter.generated.h"

UENUM(BlueprintType)
enum class EHandAnim : uint8
{
	Open,
	Grip,
	ObjectHold
};

UENUM(BlueprintType)
enum class EObjectHoldType : uint8
{
	SwordHold,
	GunHold,
	Basic,
	BasicLeft,
	None
};

DECLARE_LOG_CATEGORY_EXTERN(LogPSVRCharacter, Log, All);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTeleportationActivated, EControllerHand, bool);

UCLASS()
class PALPATINESIMULATOR_API APS_VRCharacter : public AVRCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APS_VRCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

	void SetupForPlatform();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Hands")
	void RightGripPressed();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Hands")
	void LeftGripPressed();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Hands")
	void RightGripReleased();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Hands")
	void LeftGripReleased();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Hands")
	void LeftActivateItem();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Hands")
	void RightActivateItem();

	// Force use
	UFUNCTION(Category = "Force")
	void LeftForceActivate();
	UFUNCTION(Category = "Force")
	void LeftForceDeactivate();
	UFUNCTION(Category = "Force")
	void RightForceActivate();
	UFUNCTION(Category = "Force")
	void RightForceDeactivate();

	UPROPERTY(EditDefaultsOnly, Category = "Force")
	UForceHandComponent* LeftHandForce;
	UPROPERTY(EditDefaultsOnly, Category = "Force")
	UForceHandComponent* RightHandForce;

	// For Windows
	UFUNCTION()
	void ActivateFPSMode(bool Enable);

	void MovePC(const FInputActionValue& Value);
	void LookPC(const FInputActionValue& Value);

	bool bFPSMode;
	float BaseTurnRate;
	float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character")
	float DefaultPlayerHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hands")
	USkeletalMeshComponent* LeftHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hands")
	USkeletalMeshComponent* RightHand;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hands")
	USphereComponent* LeftGrabSphere;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hands")
	USphereComponent* RightGrabSphere;

	UPROPERTY(BlueprintReadWrite, Category = "Hands|Animation")
	bool LeftGripAnim;
	UPROPERTY(BlueprintReadWrite, Category = "Hands|Animation")
	bool RightGripAnim;

	/********************************* Teleport *********************************/
	UFUNCTION(BlueprintCallable)
	void LeftTeleportPressed(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void LeftTeleportReleased(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void RightTeleportPressed(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void RightTeleportReleased(const FInputActionValue& Value);

	virtual bool CanTeleport() const;

	UFUNCTION()
	void ExecuteTeleportation(EControllerHand Hand);

	// Activate/Deactivate the Teleporter.
	UFUNCTION(BlueprintCallable, Category = "PixoVR")
	void ActivateTeleporter(EControllerHand Hand, bool InActivate);

	void SpawnTeleporter(EControllerHand Hand);

	UPROPERTY(BlueprintReadOnly, Category = "Teleport")
	APS_Teleporter* TeleportControllerLeft;
	UPROPERTY(BlueprintReadOnly, Category = "Teleport")
	APS_Teleporter* TeleportControllerRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport")
	FName TeleporterSocket;

	// Allows for disabling of Teleporter via BP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport")
	bool bTeleporterEnabled = true;

	FOnTeleportationActivated OnTeleportationActivatedEvent;

protected:
	void ActivateItemInHand(UGripMotionControllerComponent* HandController);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* InputMapping;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UPS_InputConfigData* InputActions;
};
