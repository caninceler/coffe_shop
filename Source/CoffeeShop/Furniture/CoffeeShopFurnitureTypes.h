// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CoffeeShopFurnitureTypes.generated.h"

/**
 * Mobilyanın ŞEKLİ — slot uyumluluğu bununla belirlenir. Bir slot belirli bir şekli
 * kabul eder; aynı şekildeki tüm ürünler (örn. RectTable_A ve RectTable_B) o slota konabilir.
 * None = oyuncu elinde bir şey taşımıyor.
 */
UENUM(BlueprintType)
enum class ECoffeeShopFurnitureShape : uint8
{
	None UMETA(DisplayName = "None"),
	RoundTable UMETA(DisplayName = "Round Table"),
	RectTable UMETA(DisplayName = "Rectangular Table"),
	Chair UMETA(DisplayName = "Chair"),
	Sofa UMETA(DisplayName = "Sofa")
};

/**
 * Bir mobilya ürününün tanımı (DataTable satırı). Her görsel tasarım bir satırdır:
 * aynı şekilde birden çok ürün olabilir (RectTable_A, RectTable_B → ikisi de RectTable şekli).
 * RowName ürünün kimliğidir (FName). Sipariş/taşıma/yerleştirme bu satıra göre çalışır.
 */
USTRUCT(BlueprintType)
struct FCoffeeShopFurnitureRow : public FTableRowBase
{
	GENERATED_BODY()

	// Slot uyumluluğu için şekil kategorisi.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Furniture")
	ECoffeeShopFurnitureShape Shape = ECoffeeShopFurnitureShape::RoundTable;

	// Yerleştirilince spawn edilecek görsel mobilya BP'si.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Furniture")
	TSoftClassPtr<AActor> FurnitureClass;

	// UI'da gösterilecek ad (ör. "Yuvarlak Masa", "Dikdörtgen Masa B").
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Furniture")
	FText DisplayName;

	// Opsiyonel fiyat (Parça 3 ekonomi için; şimdilik kullanılmıyor).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Furniture")
	int32 Price = 0;
};
