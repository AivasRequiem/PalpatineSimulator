// Copyright(c)  AivasGroup. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnhancedInput/Public/InputAction.h"
#include "PS_InputConfigData.generated.h"

UCLASS()
class PALPATINESIMULATOR_API UPS_InputConfigData : public UDataAsset
{
	GENERATED_BODY()
 
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputMovePC;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputLookPC;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* LeftGrip;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* RightGrip;
};
