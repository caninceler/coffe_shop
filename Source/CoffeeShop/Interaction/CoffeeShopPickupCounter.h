// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/CoffeeShopInteractable.h"
#include "CoffeeShopPickupCounter.generated.h"

class ACoffeeShopCustomerServicePoint;
class USceneComponent;

UCLASS(Blueprintable, BlueprintType)
class COFFEESHOP_API ACoffeeShopPickupCounter : public AActor, public ICoffeeShopInteractable
{
	GENERATED_BODY()

public:
	ACoffeeShopPickupCounter();

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation() const override;
	virtual void OnInteractionFocusChanged_Implementation(bool bFocused) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup Counter")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Pickup Counter")
	TObjectPtr<ACoffeeShopCustomerServicePoint> ServicePoint;
};
