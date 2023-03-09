// Copyright(c)  AivasGroup. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VRCharacter.h"
#include "PS_InputConfigData.h"
#include "EnhancedInput/Public/InputMappingContext.h"
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

	void SetupForPlatform();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Hands")
	void RightGripPressed(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Hands")
	void LeftGripPressed(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Hands")
	void RightGripReleased(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Hands")
	void LeftGripReleased(const FInputActionValue& Value);

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
	
	UPROPERTY(BlueprintReadWrite, Category = "Hands|Animation")
	bool LeftGripAnim;
	UPROPERTY(BlueprintReadWrite, Category = "Hands|Animation")
	bool RightGripAnim;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* InputMapping;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UPS_InputConfigData* InputActions;
};
