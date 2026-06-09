// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CoffeeShopPlayerController.generated.h"

UCLASS()
class COFFEESHOP_API ACoffeeShopPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACoffeeShopPlayerController();

	virtual void PlayerTick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "Interaction")
	AActor* GetFocusedInteractable() const;

protected:
	virtual void SetupInputComponent() override;

	void Interact();
	void UpdateFocusedInteractable();
	void SetFocusedInteractable(AActor* NewFocusedActor);
	void SetActorHighlight(AActor* Actor, bool bHighlighted) const;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionDistance = 500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	uint8 FocusStencilValue = 1;

	UPROPERTY()
	TObjectPtr<AActor> FocusedInteractable;
};
