// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoffeeShopTableArea.generated.h"

class ACoffeeShopFurnitureSlot;
class ACoffeeShopCustomerSeat;
class USceneComponent;

/**
 * Bir masa düzeni alanı: 1 masa slotu + N sandalye slotu + her sandalye slotuna
 * eşlenen bir oturma noktası (CoffeeShopCustomerSeat). Editörde bu referanslar atanır.
 *
 * Kural: masa slotu DOLU ve en az bir sandalye slotu DOLU ise, dolu sandalyelerin
 * Seat'leri aktive edilir (müşteri oturabilir). Aksi halde ilgili Seat'ler pasif.
 * Mevcut oturma/take-away akışı korunur — sadece Seat aktifliğini bu area yönetir.
 */
UCLASS(Blueprintable, BlueprintType)
class COFFEESHOP_API ACoffeeShopTableArea : public AActor
{
	GENERATED_BODY()

public:
	ACoffeeShopTableArea();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

	// Slot doluluk durumuna göre Seat'leri aktive/pasive eder.
	void RefreshSeats();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table Area")
	TObjectPtr<USceneComponent> Root;

	// Masanın yerleşeceği slot.
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Table Area")
	TObjectPtr<ACoffeeShopFurnitureSlot> TableSlot;

	// Sandalye slotları (sırayla SeatPoints ile eşleşir: ChairSlots[i] ↔ SeatPoints[i]).
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Table Area")
	TArray<TObjectPtr<ACoffeeShopFurnitureSlot>> ChairSlots;

	// Her sandalye slotuna karşılık gelen oturma noktası.
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Table Area")
	TArray<TObjectPtr<ACoffeeShopCustomerSeat>> SeatPoints;
};
