// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CoffeeShopInteractable.generated.h"

UINTERFACE(Blueprintable, BlueprintType)
class COFFEESHOP_API UCoffeeShopInteractable : public UInterface
{
	GENERATED_BODY()
};

class COFFEESHOP_API ICoffeeShopInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void Interact(AActor* Interactor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	FText GetInteractionPrompt() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void OnInteractionFocusChanged(bool bFocused);
};
