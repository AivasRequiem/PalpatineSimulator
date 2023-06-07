// Copyright(c) AivasGroup. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableItem.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UInteractableItem : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for interactable items (for example, activation in hand) 
 */
class PALPATINESIMULATOR_API IInteractableItem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void ActivateItem();
};
