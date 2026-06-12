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
	// Bardağın bırakılacağı/duracağı nokta (NPC buradan alır).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup Counter")
	TObjectPtr<USceneComponent> Root;

	// Bardağın yerleşeceği yer (boşsa Root kullanılır).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup Counter")
	TObjectPtr<USceneComponent> CupPoint;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Pickup Counter")
	TObjectPtr<ACoffeeShopCustomerServicePoint> ServicePoint;

	// Tezgahta bekleyen (oyuncunun bıraktığı) bardak. NPC alınca yok edilir.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Pickup Counter")
	TObjectPtr<AActor> WaitingCup;

	// NPC bekliyorsa, bardak bırakıldıktan ne kadar sonra "alınsın" (saniye).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Counter", meta = (ClampMin = "0.0"))
	float PickupDelaySeconds = 0.6f;

	FTimerHandle PickupTimerHandle;

	// Bekleyen bardağı NPC'ye "ver": serve + bardağı yok et.
	void CompletePickup();
};
