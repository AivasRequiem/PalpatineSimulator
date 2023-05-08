// Copyright(c)  AivasGroup. All Rights Reserved.

#include "GrippableFunctionLibrary.h"
#include "VRBPDatatypes.h"
#include "VRGripInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


void UGrippableFunctionLibrary::DropObject(UGripMotionControllerComponent* CallingController)
{
	TArray<FBPActorGripInformation> GripsArray;
	CallingController->GetAllGrips(GripsArray);
	for (auto GripInformation : GripsArray)
	{
		if (GripInformation.GrippedObject->IsValidLowLevel())
		{
			if (GripInformation.GrippedObject->Implements<UVRGripInterface>())
			{
				USceneComponent* SocketParent;
				FName OptionalSocketName;
				FTransform_NetQuantize SocketTransform;
				const bool IsSocket = IVRGripInterface::Execute_RequestsSocketing(GripInformation.GrippedObject, SocketParent, OptionalSocketName, SocketTransform);
				if (IsSocket)
				{
					CallingController->DropAndSocketGrip(GripInformation, SocketParent, OptionalSocketName, SocketTransform);
				}
				else
				{
					CallingController->DropObjectByInterface(GripInformation.GrippedObject, GripInformation.GripID);
				}
			}
		}
	}
}

void UGrippableFunctionLibrary::GetNearestOverlappingObject(const USphereComponent* GrabSphere, FOverlappingInfo& OverlappingInfo)
{
	if (!GrabSphere->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Warning, TEXT("UGrippableFunctionLibrary::GetNearestOverlappingObject: GrabSphere is invalid"));
		return;
	}


	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	bool TraceComplex = true;
	bool IgnoreSelf = true;
	TArray<FHitResult> HitResults;
	TArray<AActor*> ActorsToIgnore;
	bool Hit = UKismetSystemLibrary::SphereTraceMultiForObjects(GrabSphere,
	                                                            GrabSphere->GetComponentLocation() - GrabSphere->
	                                                            GetForwardVector() * 0.01,
	                                                            GrabSphere->GetComponentLocation() + GrabSphere->
	                                                            GetForwardVector() * 0.01,
	                                                            GrabSphere->GetUnscaledSphereRadius(),
	                                                            ObjectTypes,
	                                                            TraceComplex,
	                                                            ActorsToIgnore,
	                                                            EDrawDebugTrace::None,
	                                                            HitResults,
	                                                            IgnoreSelf);

	
	if (Hit)
	{
		for (auto HitResult : HitResults)
		{
			if (HitResult.Component->IsValidLowLevel())
			{
				// Does the actor support our grip interface?
				if (HitResult.Component->Implements<UVRGripInterface>())
				{
					OverlappingInfo.ObjectToGrip = HitResult.Component.Get();
					OverlappingInfo.ObjectTransform = HitResult.Component->GetComponentTransform();
				}
				else if (HitResult.GetActor()->Implements<UVRGripInterface>())
				{
					OverlappingInfo.ObjectToGrip = HitResult.GetActor();
					OverlappingInfo.ObjectTransform = HitResult.GetActor()->GetTransform();
				}
			}
			
			if (OverlappingInfo.ObjectToGrip != nullptr)
			{
				OverlappingInfo.Socket = HitResult.BoneName;
				OverlappingInfo.ImpactLocation = HitResult.Location;
			}
		}
	}
}

bool UGrippableFunctionLibrary::CanGripObject(UObject* ObjectToGrip)
{
	TArray<FBPGripPair> HoldingControllers;
	bool IsHeld;
	IVRGripInterface::Execute_IsHeld(ObjectToGrip, HoldingControllers, IsHeld);
	bool AllowsMultipleGrips = IVRGripInterface::Execute_AllowsMultipleGrips(ObjectToGrip);

	if (!AllowsMultipleGrips && IsHeld)
		return false;
	else
		return true;
}

bool UGrippableFunctionLibrary::CanSecondaryGripObject(UObject* ObjectToGrip, ESecondaryGripType& SecondaryGripType)
{
	TArray<FBPGripPair> HoldingControllers;
	bool IsHeld;
	IVRGripInterface::Execute_IsHeld(ObjectToGrip, HoldingControllers, IsHeld);
	const bool AllowsMultipleGrips = IVRGripInterface::Execute_AllowsMultipleGrips(ObjectToGrip);

	if (!IsHeld)
		return false;

	SecondaryGripType = IVRGripInterface::Execute_SecondaryGripType(ObjectToGrip);
	return SecondaryGripType != ESecondaryGripType::SG_None && AllowsMultipleGrips;
}

FTransform UGrippableFunctionLibrary::GetBoneTransform(UObject* Object, FName Socket)
{
	if (!Socket.IsNone())
	{
		if (const USceneComponent* SceneComponentTemp = Cast<USceneComponent>(Object))
		{
			return SceneComponentTemp->GetSocketTransform(Socket);
		}
		if (const USkeletalMeshComponent* SkeletalComponentTemp = Cast<USkeletalMeshComponent>(Object))
		{
			return SkeletalComponentTemp->GetSocketTransform(Socket);
		}
	}
	return FTransform();
}

bool UGrippableFunctionLibrary::GripObject(UGripMotionControllerComponent* CallingController, UGripMotionControllerComponent* OtherController, USphereComponent* GrabSphere)
{	
	FBPActorGripInformation SecondaryAttachment;
	if (OtherController->GetIsSecondaryAttachment(CallingController, SecondaryAttachment))
		return true;

	FOverlappingInfo OverlappingInfo;
	GetNearestOverlappingObject(GrabSphere, OverlappingInfo);

	if (OverlappingInfo.ObjectToGrip->IsValidLowLevel())
	{
		if (CanGripObject(OverlappingInfo.ObjectToGrip))
		{
			TArray<FBPGripPair> HoldingControllers;
			bool IsHeld;
			IVRGripInterface::Execute_IsHeld(OverlappingInfo.ObjectToGrip, HoldingControllers, IsHeld);
			
			if (!IsHeld)
			{
				FName GripSlot;
				if (OverlappingInfo.Socket.IsNone())
					GripSlot = "VRGripP";
				else
					GripSlot = FName(OverlappingInfo.Socket.ToString() + "VRGripP");

				bool HasSlot;
				FTransform SlotTransform;
				FName SlotName;
				IVRGripInterface::Execute_ClosestGripSlotInRange(OverlappingInfo.ObjectToGrip, OverlappingInfo.ImpactLocation, false, HasSlot, SlotTransform, SlotName, CallingController, GripSlot);

				FTransform PlainOrGripTransform = OverlappingInfo.Socket.IsNone() ? OverlappingInfo.ObjectTransform : GetBoneTransform(OverlappingInfo.ObjectToGrip, OverlappingInfo.Socket);
				FTransform GripTransform = HasSlot ? UKismetMathLibrary::MakeRelativeTransform(PlainOrGripTransform, SlotTransform) : CallingController->ConvertToControllerRelativeTransform(PlainOrGripTransform);

				CallingController->GripObjectByInterface(OverlappingInfo.ObjectToGrip, GripTransform, true, OverlappingInfo.Socket, SlotName, HasSlot);
			}
			else
			{
				ESecondaryGripType SecondaryGripType = ESecondaryGripType::SG_None;
				if (CanSecondaryGripObject(OverlappingInfo.ObjectToGrip, SecondaryGripType))
				{
					bool HasSlot;
					FTransform SlotTransform;
					FName SlotName;
					IVRGripInterface::Execute_ClosestGripSlotInRange(OverlappingInfo.ObjectToGrip, OverlappingInfo.ImpactLocation, true, HasSlot, SlotTransform, SlotName, CallingController, FName());
					
					FTransform GripTransform = HasSlot ? UGripMotionControllerComponent::ConvertToGripRelativeTransform(OverlappingInfo.ObjectTransform, SlotTransform) : UGripMotionControllerComponent::ConvertToGripRelativeTransform(OverlappingInfo.ObjectTransform, CallingController->GetPivotTransform());

					CallingController->AddSecondaryAttachmentPoint(OverlappingInfo.ObjectToGrip, OtherController, GripTransform, true, 0.25, HasSlot, SlotName);
				}
			}
		}
	}
	
	return false;
}
