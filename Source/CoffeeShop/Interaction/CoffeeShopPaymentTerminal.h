// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/CoffeeShopInteractable.h"
#include "CoffeeShopPaymentTerminal.generated.h"

class ACoffeeShopCustomerServicePoint;
class USceneComponent;

UCLASS(Blueprintable, BlueprintType)
class COFFEESHOP_API ACoffeeShopPaymentTerminal : public AActor, public ICoffeeShopInteractable
{
	GENERATED_BODY()

public:
	ACoffeeShopPaymentTerminal();

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation() const override;
	virtual void OnInteractionFocusChanged_Implementation(bool bFocused) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Payment Terminal")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Payment Terminal")
	TObjectPtr<ACoffeeShopCustomerServicePoint> ServicePoint;
};
