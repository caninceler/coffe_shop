// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CoffeeShopCustomerTypes.generated.h"

UENUM(BlueprintType)
enum class ECoffeeShopCustomerVisitState : uint8
{
	None UMETA(DisplayName = "None"),
	Entering UMETA(DisplayName = "Entering"),
	WalkingToCounter UMETA(DisplayName = "Walking To Counter"),
	Ordering UMETA(DisplayName = "Ordering"),
	WaitingForDrink UMETA(DisplayName = "Waiting For Drink"),
	LookingForSeat UMETA(DisplayName = "Looking For Seat"),
	WalkingToSeat UMETA(DisplayName = "Walking To Seat"),
	Seated UMETA(DisplayName = "Seated"),
	Leaving UMETA(DisplayName = "Leaving"),
	Angry UMETA(DisplayName = "Angry")
};

USTRUCT(BlueprintType)
struct FCoffeeShopCustomerOrderRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Order")
	FName DrinkId = TEXT("BasicCoffee");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Order")
	FText DisplayName;
};
