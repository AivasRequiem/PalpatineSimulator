// Copyright(c) AivasGroup. All Rights Reserved.

#include "PS_Teleporter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "VRCharacter.h"
#include "VRExpansionFunctionLibrary.h"
#include "VRRootComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogPSTeleporter);

APS_Teleporter::APS_Teleporter()
	: ArcSplineComponent(nullptr)
	  , ArcEndPoint(nullptr)
	  , TeleportCylinder(nullptr)
	  , Ring(nullptr)
	  , Arrow(nullptr)
	  , ArcSplineMesh(nullptr)
	  , ArcSplineMaterial(nullptr)
	  , MotionController(nullptr)
	  , bIsTeleporterActive(false)
	  , bIsValidTeleportDestination(false)
	  , bDrawTeleportArrow(false)
	  , bDrawArcSpline(true)
	  , bDrawTeleportCylinder(true)
	  , TeleportRotation(FRotator::ZeroRotator)
	  , PadRotation(FRotator::ZeroRotator)
	  , ThumbDeadZone(0.5f)
	  , TeleportLaunchVelocity(1200.0f)
	  , FadeInDuration(0.25f)
	  , FadeOutDuration(0.25f)
	  , TeleportFadeColor(FLinearColor::Black)
	  , NavigationSystem(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;
	SetReplicateMovement(false);
	bNetLoadOnClient = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ArcSplineMeshFinder(
		TEXT("StaticMesh'/Game/Assets/Meshes/Teleporter/SM_BeamMesh.SM_BeamMesh'"));
	ArcSplineMesh = ArcSplineMeshFinder.Object;

	static ConstructorHelpers::FObjectFinder<UMaterial> ArcSplineMaterialFinder(
		TEXT("/Script/Engine.Material'/Game/Assets/Materials/Teleporter/M_SplineArc.M_SplineArc'"));
	ArcSplineMaterial = ArcSplineMaterialFinder.Object;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ArcEndPointMeshFinder(
		TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	UStaticMesh* ArcEndPointMesh = ArcEndPointMeshFinder.Object;

	static ConstructorHelpers::FObjectFinder<UMaterial> ArcEndPointMaterialFinder(
		TEXT("/Script/Engine.Material'/Game/Assets/Materials/Teleporter/M_ArcEndpoint.M_ArcEndpoint'"));
	UMaterial* ArcEndPointMaterial = ArcEndPointMaterialFinder.Object;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMeshFinder(
		TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	UStaticMesh* CylinderMesh = CylinderMeshFinder.Object;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderRingMeshFinder(
		TEXT("StaticMesh'/Game/Assets/Meshes/Teleporter/SM_FatCylinder.SM_FatCylinder'"));
	UStaticMesh* CylinderRingMesh = CylinderRingMeshFinder.Object;

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> TeleportCylinderMaterialFinder(
		TEXT(
			"/Script/Engine.MaterialInstanceConstant'/Game/Assets/Materials/Teleporter/MI_TeleportCylinderPreview.MI_TeleportCylinderPreview'"));
	UMaterialInstance* CylinderMaterial = TeleportCylinderMaterialFinder.Object;

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TeleportSceneComponent"));
	RootComponent = SceneComponent;

	// Arc Spline.
	ArcSplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("ArcSpline"));
	ArcSplineComponent->SetupAttachment(SceneComponent);
	ArcSplineComponent->SetWorldLocation(FVector(12.53f, -1.76f, 2.55f));

	// Arc Endpoint.
	ArcEndPoint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArcEndPoint"));
	ArcEndPoint->SetupAttachment(SceneComponent);
	ArcEndPoint->SetWorldScale3D(FVector(0.08f, 0.08f, 0.08f));
	ArcEndPoint->SetStaticMesh(ArcEndPointMesh);
	ArcEndPoint->SetMaterial(0, ArcEndPointMaterial);
	ArcEndPoint->SetVisibility(false, true);
	ArcEndPoint->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	ArcEndPoint->SetGenerateOverlapEvents(false);
	ArcEndPoint->SetAbsolute(true, true, true);

	// Teleport Cylinder.
	TeleportCylinder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportCylinder"));
	TeleportCylinder->SetupAttachment(SceneComponent);
	TeleportCylinder->SetWorldScale3D(FVector(0.75f, 0.75f, 1.0f));
	TeleportCylinder->SetStaticMesh(CylinderMesh);
	TeleportCylinder->SetMaterial(0, CylinderMaterial);
	TeleportCylinder->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	TeleportCylinder->SetGenerateOverlapEvents(false);
	TeleportCylinder->SetAbsolute(true, true, true);

	// Teleport Cylinder Ring.
	Ring = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ring"));
	Ring->SetupAttachment(TeleportCylinder);
	Ring->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.15f));
	Ring->SetStaticMesh(CylinderRingMesh);
	Ring->SetMaterial(0, ArcEndPointMaterial);
	Ring->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	Ring->SetGenerateOverlapEvents(false);

	// Teleport Cylinder Arrow.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderArrowMeshFinder(
		TEXT("StaticMesh'/Game/Assets/Meshes/Teleporter/SM_BeaconDirection.SM_BeaconDirection'"));
	UStaticMesh* CylinderArrowMesh = CylinderArrowMeshFinder.Object;

	Arrow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(SceneComponent);
	Arrow->SetStaticMesh(CylinderArrowMesh);
	Arrow->SetMaterial(0, ArcEndPointMaterial);
	Arrow->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	Arrow->SetGenerateOverlapEvents(false);
	Arrow->SetVisibility(false, true);
}

bool APS_Teleporter::IsActivated()
{
	return bIsTeleporterActive;
}

void APS_Teleporter::BeginPlay()
{
	Super::BeginPlay();

	TeleportCylinder->SetVisibility(false, true);

	NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
}

void APS_Teleporter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsTeleporterActive)
	{
		TickTeleporter(DeltaTime);
	}
}

void APS_Teleporter::Destroyed()
{
	Super::Destroyed();

	ClearArc();
}

void APS_Teleporter::TickTeleporter(float DeltaTime)
{
	TArray<FVector> TracePoints;

	// Is the location of the projectile falling onto the ground.
	FVector TraceLocation;

	// The projects location will be mapped to the Nav Mesh location.
	FVector NavMeshLocation;

	bIsValidTeleportDestination = TraceTeleportDestination(TracePoints, TraceLocation, NavMeshLocation);

	if (bDrawArcSpline)
	{
		UpdateArcSpline(TracePoints, bIsValidTeleportDestination);
	}

	if (bDrawTeleportCylinder)
	{
		TeleportCylinder->SetVisibility(bIsValidTeleportDestination, true);
	}

	UpdateTeleportCylinder(NavMeshLocation, bIsValidTeleportDestination);
	UpdateArcEndpoint(TraceLocation, bIsValidTeleportDestination);

	// We prepare the rotation of the teleport cylinder.
	if (PadRotation != FRotator::ZeroRotator)
	{
		// User changed rotation using the otion Controller Pad.
		TeleportRotation = PadRotation;
	}
	else
	{
		// If we don't have any Motion Controller Pad rotation, set pitch and roll to zero.
		// and use previous Yaw rotation.
		TeleportRotation.Pitch = 0.0f;
		TeleportRotation.Roll = 0.0f;
	}
}

void APS_Teleporter::SetMotionController(UGripMotionControllerComponent* InMotionController)
{
	MotionController = InMotionController;
}

void APS_Teleporter::ActivateTeleporter(bool InActivate)
{
	if (InActivate == bIsTeleporterActive)
	{
		return;
	}

	bIsTeleporterActive = InActivate;

	// Enable/Disable ticking for this actor.
	SetActorTickEnabled(bIsTeleporterActive);

	// Hide/Show the teleport cylinder.
	if (bDrawTeleportCylinder)
	{
		TeleportCylinder->SetVisibility(InActivate, true);
	}

	// Hide/Show the arc splines.
	ArcSplineComponent->SetVisibility(InActivate && bDrawArcSpline, true);

	// Handle visibility of the arrow if it is enabled.
	Arrow->SetVisibility(InActivate && bDrawTeleportArrow, true);

	// The ArchEndPoint is activated inside UpdateArchEndPoint. We deactivate definitely when
	// InActivate = false.
	if (!InActivate)
	{
		ArcEndPoint->SetVisibility(false, false);
		ClearArc();
	}
}

void APS_Teleporter::ClearArc()
{
	for (const auto& SplineMeshComponent : SplineMeshComponents)
	{
		SplineMeshComponent->DestroyComponent();
	}
	SplineMeshComponents.Empty();

	ArcSplineComponent->ClearSplinePoints(true);
}

bool APS_Teleporter::TraceTeleportDestination(TArray<FVector>& TracePoints, FVector& TraceLocation,
                                              FVector& NavMeshLocation)
{
	FVector StartPos(FVector::ZeroVector);
	FVector LaunchVelocity(FVector::ZeroVector);

	GetTeleportationTransform(StartPos, LaunchVelocity);
	LaunchVelocity *= TeleportLaunchVelocity;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

	FPredictProjectilePathParams Params;
	Params.bTraceWithCollision = true;
	Params.bTraceComplex = false;
	Params.DrawDebugType = EDrawDebugTrace::None;
	Params.DrawDebugTime = 0.0f;
	Params.SimFrequency = 30.0f;
	Params.OverrideGravityZ = 0.0f;
	Params.ObjectTypes = ObjectTypes;
	Params.bTraceWithChannel = false;
	Params.ProjectileRadius = 0.0f;
	Params.StartLocation = StartPos;
	Params.MaxSimTime = 2.0f;
	Params.LaunchVelocity = LaunchVelocity;

	FPredictProjectilePathResult PredictResult;
	bool bHit = UGameplayStatics::PredictProjectilePath(this, Params, PredictResult);

	FHitResult HitResult;
	FVector LastTraceDestination;

	HitResult = PredictResult.HitResult;
	LastTraceDestination = PredictResult.LastTraceDestination.Location;
	TracePoints.Empty(PredictResult.PathData.Num());

	for (const FPredictProjectilePathPointData& PathPoint : PredictResult.PathData)
	{
		TracePoints.Add(PathPoint.Location);
	}

	TraceLocation = HitResult.Location;

	bool bProjectPointToNavigation = false;
	if (NavigationSystem)
	{
		FVector QueryingExtent(500.0f, 500.0f, 500.0f);
		FNavLocation NavLocation{};

		bProjectPointToNavigation = NavigationSystem->ProjectPointToNavigation(
			HitResult.Location, NavLocation, QueryingExtent);
		NavMeshLocation = NavLocation.Location;
	}

	bProjectPointToNavigation = (NavMeshLocation != FVector(0.0f, 0.0f, 0.0f)) && (TraceLocation != NavMeshLocation);

	return bHit & bProjectPointToNavigation;
}

void APS_Teleporter::GetTeleportationTransform(FVector& Location, FVector& ForwardVector)
{
	Location = RootComponent->GetComponentLocation();
	ForwardVector = RootComponent->GetForwardVector();
}

FVector APS_Teleporter::GetTeleportDestination(const FVector& OriginalLocation)
{
	AVRCharacter* VRCharacter = Cast<AVRCharacter>(GetOwner());
	FVector Offset = VRCharacter->VRRootReference->OffsetComponentToWorld.GetLocation() - VRCharacter->
		GetActorLocation();
	Offset.Z = 0.0f;
	return OriginalLocation - Offset;
}

void APS_Teleporter::UpdateMotionControllerRotation(const FVector2D& Direction)
{
	if (!bIsTeleporterActive && !bDrawTeleportArrow)
	{
		return;
	}

	PadRotation = GetRotationFromInput(Direction);
}

FRotator APS_Teleporter::GetRotationFromInput(const FVector2D& Direction)
{
	// Rotate input X+Y always point forward relative to the current pawn rotation.
	FVector A(Direction.X, Direction.Y, 0.0f);

	// We should normalize the vector.
	A.Normalize();

	// Check whether thumb in near center (ignore rotation overrides).
	const bool Condition = A.Size() > ThumbDeadZone;
	if (Condition)
	{
		FRotator CharacterRotation = GetOwner()->GetActorRotation();
		CharacterRotation.Roll = 0.0f;
		CharacterRotation.Pitch = 0.0f;

		const FVector RotatedVector = CharacterRotation.RotateVector(A);
		return RotatedVector.Rotation();
	}

	return FRotator::ZeroRotator;
}

void APS_Teleporter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APS_Teleporter, MotionController);
}

void APS_Teleporter::UpdateTeleportCylinder(const FVector& NavMeshLocation, bool ValidLocationFound)
{
	if (ValidLocationFound)
	{
		const auto Start = NavMeshLocation;
		const auto End = Start + FVector(0.0f, 0.0f, -200.0f);
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
		const auto TraceComplex = false;
		const TArray<AActor*> ActorsToIgnore;
		const auto IgnoreSelf = true;
		FHitResult HitResult;
		const float DrawTime = 5.0;

		UKismetSystemLibrary::LineTraceSingleForObjects(this,
		                                                Start,
		                                                End,
		                                                ObjectTypes,
		                                                TraceComplex,
		                                                ActorsToIgnore,
		                                                EDrawDebugTrace::None,
		                                                HitResult,
		                                                IgnoreSelf,
		                                                FLinearColor::Red,
		                                                FLinearColor::Green,
		                                                DrawTime);

		FVector NewLocation = FVector::ZeroVector;
		if (HitResult.bBlockingHit)
		{
			NewLocation = HitResult.ImpactPoint;
		}
		else
		{
			NewLocation = NavMeshLocation;
		}

		TeleportCylinder->SetWorldLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
	}
	else
	{
		// TODO Do we want to do something here if we have no valid location?
	}
}

void APS_Teleporter::UpdateArcEndpoint(const FVector& NewLocation, bool ValidLocationFound)
{
	ArcEndPoint->SetVisibility(ValidLocationFound, true);
	ArcEndPoint->SetWorldLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
}

void APS_Teleporter::UpdateArcSpline(TArray<FVector>& SplinePoints, bool ValidLocationFound)
{
	ArcSplineComponent->ClearSplinePoints(true);

	if (ValidLocationFound)
	{
		for (auto& SplinePoint : SplinePoints)
		{
			ArcSplineComponent->AddSplinePoint(SplinePoint, ESplineCoordinateSpace::Local, true);
		}

		ArcSplineComponent->SetSplinePointType(SplinePoints.Num() - 1, ESplinePointType::CurveClamped, true);

		if (SplineMeshComponents.Num() < ArcSplineComponent->GetNumberOfSplinePoints())
		{
			const int32 NumberOfSplinesToAdd = (ArcSplineComponent->GetNumberOfSplinePoints() - 1) -
				SplineMeshComponents.Num();

			for (int32 Index = 0; Index <= NumberOfSplinesToAdd; Index++)
			{
				USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this);

				SplineMeshComponent->SetMobility(EComponentMobility::Movable);
				SplineMeshComponent->SetStartPosition(FVector(0.0f, 0.0f, 0.0f), true);
				SplineMeshComponent->SetStartTangent(FVector(100.0f, 0.0f, 0.0f), true);
				SplineMeshComponent->SetEndPosition(FVector(100.0f, 0.0f, 0.0f), true);
				SplineMeshComponent->SetEndTangent(FVector(100.0f, 0.0f, 0.0f), true);
				SplineMeshComponent->SetSplineUpDir(FVector(0.0f, 0.0f, 1.0f), true);
				SplineMeshComponent->SetForwardAxis(ESplineMeshAxis::X, true);
				SplineMeshComponent->SetStartScale(FVector2D(4, 4));
				SplineMeshComponent->SetEndScale(FVector2D(4, 4));
				SplineMeshComponent->SetStaticMesh(ArcSplineMesh);
				SplineMeshComponent->SetMaterial(0, ArcSplineMaterial);
				SplineMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				SplineMeshComponent->SetGenerateOverlapEvents(false);

				FTransform Transform = FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f),
				                                  FVector(1.0f, 1.0f, 1.0f));
				SplineMeshComponent->SetRelativeTransform(Transform);
				SplineMeshComponent->RegisterComponent();

				SplineMeshComponents.Add(SplineMeshComponent);
			}
		}

		// Update the start/end points of the spline components.
		for (int32 Index = 0; Index < SplineMeshComponents.Num(); Index++)
		{
			USplineMeshComponent* SplineMeshComponent = SplineMeshComponents[Index];

			if (Index < (ArcSplineComponent->GetNumberOfSplinePoints() - 1))
			{
				SplineMeshComponent->SetVisibility(true, false);

				const auto StartTangent = ArcSplineComponent->GetTangentAtSplinePoint(
					Index, ESplineCoordinateSpace::Local);
				const auto EndTangent = ArcSplineComponent->GetTangentAtSplinePoint(
					Index + 1, ESplineCoordinateSpace::Local);

				SplineMeshComponent->SetStartAndEnd(SplinePoints[Index], StartTangent, SplinePoints[Index + 1],
				                                    EndTangent, true);
			}
			else
			{
				// Unused spline components we better hide.
				SplineMeshComponent->SetVisibility(false, false);
			}
		}
	}
	else
	{
		// TODO For now we just hide all splines when we don't have a valid teleport location.
		for (int32 Index = 0; Index < SplineMeshComponents.Num(); Index++)
		{
			USplineMeshComponent* SplineMeshComponent = SplineMeshComponents[Index];
			SplineMeshComponent->SetVisibility(false, false);
		}
	}
}

#if WITH_EDITOR
void APS_Teleporter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property != nullptr)
	{
		const FName PropertyName(PropertyChangedEvent.Property->GetFName());
		if (PropertyName == GET_MEMBER_NAME_CHECKED(APS_Teleporter, bDrawTeleportArrow))
		{
			Arrow->SetVisibility(bDrawTeleportArrow, false);
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void APS_Teleporter::ExecuteTeleportation(UVRBaseCharacterMovementComponent* InMovementComponent,
                                          const FTransform& ActorTransform, const FVector& VRLocation,
                                          const FRotator& ActorRotation)
{
	// If we don't have a valid destination, don't execute teleportation.
	if (!bIsValidTeleportDestination)
	{
		return;
	}

	MovementComponent = InMovementComponent;

	const FVector PivotPoint = VRLocation;
	const FVector InvPivotPoint = UKismetMathLibrary::InverseTransformLocation(ActorTransform, PivotPoint);

	const FVector LastValidTeleportLocation = TeleportCylinder->GetComponentLocation();
	const FVector TeleportToLocation = GetTeleportDestination(LastValidTeleportLocation);

	UVRExpansionFunctionLibrary::RotateAroundPivot(FRotator::ZeroRotator, TeleportToLocation, ActorRotation,
	                                               InvPivotPoint, TimedLocation, TimedRotation, true);

	MovementComponent->PerformMoveAction_Teleport(TimedLocation, TimedRotation);
}
