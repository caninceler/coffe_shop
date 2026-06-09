// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoffeeShopCustomerSeat.generated.h"

class USceneComponent;

UCLASS(Blueprintable, BlueprintType)
class COFFEESHOP_API ACoffeeShopCustomerSeat : public AActor
{
	GENERATED_BODY()

public:
	ACoffeeShopCustomerSeat();

	UFUNCTION(BlueprintCallable, Category = "Customer Seat")
	bool ClaimSeat(AActor* Customer);

	UFUNCTION(BlueprintCallable, Category = "Customer Seat")
	void ReleaseSeat(AActor* Customer);

	UFUNCTION(BlueprintPure, Category = "Customer Seat")
	bool IsAvailable() const;

	UFUNCTION(BlueprintPure, Category = "Customer Seat")
	FTransform GetSitTransform() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Customer Seat")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Customer Seat")
	TObjectPtr<USceneComponent> SitPoint;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Customer Seat")
	TObjectPtr<AActor> OccupyingCustomer;
};
