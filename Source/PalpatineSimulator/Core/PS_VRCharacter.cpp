// Copyright(c)  AivasGroup. All Rights Reserved.


#include "PS_VRCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GripMotionControllerComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "VRCharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LogPSVRCharacter);

// Sets default values
APS_VRCharacter::APS_VRCharacter() : BaseTurnRate(45.0f), BaseLookUpRate(45.0f), DefaultPlayerHeight(165.0f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LeftHand = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Left Hand"));
	LeftHand->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
	LeftHand->SetRelativeScale3D(FVector(1.0f, 1.0f, -1.0f));
	LeftHand->SetupAttachment(LeftMotionController);
	
	RightHand = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Right Hand"));
	RightHand->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
	RightHand->SetupAttachment(RightMotionController);
	
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	VRReplicatedCamera->bUsePawnControlRotation = false;
	VRReplicatedCamera->bLockToHmd = true;
	VRReplicatedCamera->bAutoSetLockToHmd = true;
	VRReplicatedCamera->SetRelativeLocation(FVector(0.0f, 0.0f, DefaultPlayerHeight));
}

// Called when the game starts or when spawned
void APS_VRCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetupForPlatform();
}

void APS_VRCharacter::SetupForPlatform()
{
	const auto DeviceName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName();

	UE_LOG(LogPSVRCharacter, Log, TEXT("APS_VRCharacter::BeginPlay DeviceName = %s"), *DeviceName.ToString());

	if (DeviceName == "SteamVR") // VIVE
	{
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
	}
	else if (DeviceName == "Oculus Quest") // Oculus
	{
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Eye);
		VRProxyComponent->AddLocalOffset(FVector(0.0f, 0.0f, DefaultPlayerHeight));
	}
	else if (DeviceName == "WaveVR") // FOCUS PLUS
	{
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Eye);
	}
	else if (DeviceName == "PICOXRHMD") // PICO NEO
	{
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
	}
	else // Windows
	{
		ActivateFPSMode(true);
	}
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

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	// Clear out existing mapping, and add our mapping
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);

	// Get the EnhancedInputComponent
	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	// Bind the actions
	PEI->BindAction(InputActions->InputMovePC, ETriggerEvent::Triggered, this, &APS_VRCharacter::MovePC);
	PEI->BindAction(InputActions->InputLookPC, ETriggerEvent::Triggered, this, &APS_VRCharacter::LookPC);

	PEI->BindAction(InputActions->LeftGrip, ETriggerEvent::Started, this, &APS_VRCharacter::LeftGripPressed);
	PEI->BindAction(InputActions->LeftGrip, ETriggerEvent::Completed, this, &APS_VRCharacter::LeftGripReleased);
	PEI->BindAction(InputActions->RightGrip, ETriggerEvent::Started, this, &APS_VRCharacter::RightGripPressed);
	PEI->BindAction(InputActions->RightGrip, ETriggerEvent::Completed, this, &APS_VRCharacter::RightGripReleased);
}

void APS_VRCharacter::ActivateFPSMode(bool Enable)
{
	bFPSMode = Enable;

	if (bFPSMode)
	{
		LeftMotionController->bUseWithoutTracking = true;
		LeftMotionController->bOffsetByControllerProfile = true;
		LeftMotionController->AttachToComponent(VRReplicatedCamera, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
		LeftMotionController->SetRelativeLocation(FVector(62.0f, -28.0f, -15.0f));
		LeftHand->SetRelativeLocation(FVector(-12.0f, 0.0f, 0.0f));
		LeftHand->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0f));

		RightMotionController->bUseWithoutTracking = true;
		RightMotionController->bOffsetByControllerProfile = true;
		RightMotionController->AttachToComponent(VRReplicatedCamera, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
		RightMotionController->SetRelativeLocation(FVector(62.0f, 31.0f, -15.0f));
		RightHand->SetRelativeLocation(FVector(-12.0f, 0.0f, 0.0f));
		RightHand->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));

		bUseControllerRotationPitch = false;

		const auto MovementComponent = Cast<UVRCharacterMovementComponent>(GetMovementComponent());
		MovementComponent->bUseClientControlRotation = true;
		MovementComponent->bAllowMovementMerging = true;

		VRReplicatedCamera->bUsePawnControlRotation = true;
		VRReplicatedCamera->bLockToHmd = false;
		VRReplicatedCamera->bAutoSetLockToHmd = false;
	}
}

void APS_VRCharacter::MovePC(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		const FVector2D MoveValue = Value.Get<FVector2D>();
 
		// Forward/Backward direction
		if (MoveValue.Y != 0.f)
		{
			AddMovementInput(GetVRForwardVector(), MoveValue.Y);
		}
 
		// Right/Left direction
		if (MoveValue.X != 0.f)
		{
			AddMovementInput(GetVRRightVector(), MoveValue.X);
		}
	}
}

void APS_VRCharacter::LookPC(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		const FVector2D LookValue = Value.Get<FVector2D>();
 
		if (LookValue.X != 0.f)
		{
			AddControllerYawInput(LookValue.X * BaseTurnRate * GetWorld()->GetDeltaSeconds());
		}
 
		if (LookValue.Y != 0.f)
		{
			AddControllerPitchInput(LookValue.Y * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
		}
	}
}

void APS_VRCharacter::LeftGripPressed_Implementation(const FInputActionValue& Value)
{
	LeftGripAnim = true;
}

void APS_VRCharacter::LeftGripReleased_Implementation(const FInputActionValue& Value)
{
	LeftGripAnim = false;
}

void APS_VRCharacter::RightGripPressed_Implementation(const FInputActionValue& Value)
{
	RightGripAnim = true;
}

void APS_VRCharacter::RightGripReleased_Implementation(const FInputActionValue& Value)
{
	RightGripAnim = false;
}
