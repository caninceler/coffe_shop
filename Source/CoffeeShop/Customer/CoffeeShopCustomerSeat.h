// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoffeeShopCustomerSeat.generated.h"

class USceneComponent;

/**
 * Müşterinin oturduğu nokta. Yerleştirme sistemi (TableArea) bir sandalye + masa
 * yerleştirilince bu koltuğu SetSeatActive(true) ile açar; pasif koltuğa kimse oturamaz.
 */
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

	// Pasif (bActive=false) koltuk hiçbir zaman müsait değildir → kimse oturamaz,
	// müşteri take away'e döner. Aktif ve boşsa müsaittir.
	UFUNCTION(BlueprintPure, Category = "Customer Seat")
	bool IsAvailable() const;

	UFUNCTION(BlueprintPure, Category = "Customer Seat")
	FTransform GetSitTransform() const;

	// Bu koltuğu aç/kapa (TableArea çağırır: masa+sandalye varsa açık).
	UFUNCTION(BlueprintCallable, Category = "Customer Seat")
	void SetSeatActive(bool bNewActive);

	UFUNCTION(BlueprintPure, Category = "Customer Seat")
	bool IsSeatActive() const;

	// Bu koltukta şu an biri oturuyor mu?
	UFUNCTION(BlueprintPure, Category = "Customer Seat")
	bool IsOccupied() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Customer Seat")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Customer Seat")
	TObjectPtr<USceneComponent> SitPoint;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Customer Seat")
	TObjectPtr<AActor> OccupyingCustomer;

	// Koltuk aktif mi? Başta pasif; TableArea masa+sandalye yerleşince açar.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Customer Seat")
	bool bActive = false;
};
