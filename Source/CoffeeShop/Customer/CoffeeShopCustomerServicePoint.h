// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoffeeShopCustomerServicePoint.generated.h"

class USceneComponent;
class ACoffeeShopCustomerCharacter;

UCLASS(Blueprintable, BlueprintType)
class COFFEESHOP_API ACoffeeShopCustomerServicePoint : public AActor
{
	GENERATED_BODY()

public:
	ACoffeeShopCustomerServicePoint();

	UFUNCTION(BlueprintPure, Category = "Customer Service")
	FTransform GetQueueTransform() const;

	UFUNCTION(BlueprintPure, Category = "Customer Service")
	FTransform GetPaymentTransform() const;

	UFUNCTION(BlueprintCallable, Category = "Customer Service")
	FVector GetPaymentQueueLocationForCustomer(ACoffeeShopCustomerCharacter* Customer);

	UFUNCTION(BlueprintPure, Category = "Customer Service")
	FTransform GetPickupTransform() const;

	UFUNCTION(BlueprintPure, Category = "Customer Service", meta = (DeprecatedFunction, DeprecationMessage = "Use GetPaymentTransform instead."))
	FTransform GetOrderTransform() const;

	UFUNCTION(BlueprintCallable, Category = "Customer Service")
	void RegisterWaitingForPayment(ACoffeeShopCustomerCharacter* Customer);

	UFUNCTION(BlueprintCallable, Category = "Customer Service")
	bool AcceptNextPayment();

	// Sıradaki (ödeme bekleyen) müşteri; kuyruk boşsa null. Onay ekranı bununla
	// müşteriyi ve siparişini gösterir.
	UFUNCTION(BlueprintPure, Category = "Customer Service|Queue")
	ACoffeeShopCustomerCharacter* GetNextPaymentCustomer() const;

	// Sıradaki müşteri ödeme noktasının önüne ulaştı mı? Onay ekranı açılmadan önce
	// kontrol edilir; AcceptNextPayment ile aynı mesafe mantığını paylaşır.
	UFUNCTION(BlueprintPure, Category = "Customer Service|Queue")
	bool IsNextPaymentCustomerAtCounter() const;

	UFUNCTION(BlueprintCallable, Category = "Customer Service")
	void RegisterWaitingForDrink(ACoffeeShopCustomerCharacter* Customer);

	UFUNCTION(BlueprintCallable, Category = "Customer Service")
	bool ServeNextDrink();

	UFUNCTION(BlueprintCallable, Category = "Customer Service")
	void RemoveCustomerFromQueues(ACoffeeShopCustomerCharacter* Customer);

	UFUNCTION(BlueprintPure, Category = "Customer Service|Queue")
	bool IsFirstWaitingForPayment(ACoffeeShopCustomerCharacter* Customer) const;

	UFUNCTION(BlueprintPure, Category = "Customer Service|Queue")
	bool CanAcceptNewCustomer() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Customer Service")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Customer Service")
	TObjectPtr<USceneComponent> QueuePoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Customer Service")
	TObjectPtr<USceneComponent> PaymentPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Customer Service")
	TObjectPtr<USceneComponent> PickupPoint;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Customer Service")
	TArray<TObjectPtr<ACoffeeShopCustomerCharacter>> PaymentQueue;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Customer Service")
	TArray<TObjectPtr<ACoffeeShopCustomerCharacter>> PickupQueue;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Customer Service")
	TObjectPtr<ACoffeeShopCustomerCharacter> ActivePickupCustomer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Service|Queue", meta = (ClampMin = "50.0", UIMin = "50.0"))
	float PaymentQueueSlotSpacing = 75.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Service|Queue", meta = (ClampMin = "1", UIMin = "1"))
	int32 MaxCustomersInServiceFlow = 6;

	// Müşteri ödeme noktasına bu mesafeden daha yakın değilse "Take Order" (E) çalışmaz.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Service|Queue", meta = (ClampMin = "10.0", UIMin = "10.0"))
	float PaymentReachDistance = 150.0f;

	void CompactQueues();
	void RefreshPaymentQueuePositions();

	// Verilen müşteri ödeme noktasına yeterince yakın mı (PaymentReachDistance içinde)?
	bool HasCustomerReachedCounter(const ACoffeeShopCustomerCharacter* Customer) const;
};
