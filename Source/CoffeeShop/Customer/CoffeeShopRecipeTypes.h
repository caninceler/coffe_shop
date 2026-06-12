// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CoffeeShopRecipeTypes.generated.h"

/**
 * Kahve hazırlamada bir istasyon adımı. Her istasyon bir adım sağlar; bir içeceğin
 * tarifi bu adımların sıralı listesidir.
 */
UENUM(BlueprintType)
enum class ECoffeeStep : uint8
{
	None UMETA(DisplayName = "None"),
	GrabCup UMETA(DisplayName = "Grab Cup"),          // bardak al
	Grind UMETA(DisplayName = "Grind"),                // çekirdek öğüt
	PullEspresso UMETA(DisplayName = "Pull Espresso"), // espresso çek
	HotWater UMETA(DisplayName = "Hot Water"),          // sıcak su (boiler)
	SteamMilk UMETA(DisplayName = "Steam Milk"),        // süt
	AddSyrup UMETA(DisplayName = "Add Syrup")           // şurup
};

/**
 * Bir içeceğin tarifi (DataTable satırı). RowName = DrinkId (müşterinin OrderRequest.DrinkId
 * ile eşleşir). Steps = sırayla yapılması gereken istasyon adımları.
 * Örn. Americano: Steps = [GrabCup, Grind, PullEspresso, HotWater].
 */
USTRUCT(BlueprintType)
struct FCoffeeRecipeRow : public FTableRowBase
{
	GENERATED_BODY()

	// İçeceğin sıralı hazırlama adımları.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	TArray<ECoffeeStep> Steps;

	// UI/log için ad (ör. "Americano").
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	FText DisplayName;
};
