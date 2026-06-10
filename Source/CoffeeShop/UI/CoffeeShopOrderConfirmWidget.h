// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Customer/CoffeeShopCustomerTypes.h"
#include "CoffeeShopOrderConfirmWidget.generated.h"

class ACoffeeShopCustomerCharacter;
class ACoffeeShopCustomerServicePoint;
class ACoffeeShopPlayerController;

/**
 * "Take Order" onay ekranının C++ tabanı. WBP_OrderConfirm bundan türetilir ve
 * mockup'a göre tasarlanır. C++ tarafı veriyi (müşteri + sipariş) sağlar ve
 * Onayla/Kapat aksiyonlarını yürütür; görsel WBP'de yapılır.
 */
UCLASS(Abstract, Blueprintable)
class COFFEESHOP_API UCoffeeShopOrderConfirmWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Ekran açılırken çağrılır: hedef service point + sıradaki müşteriyi bağlar,
	// siparişi okur ve WBP'nin metinleri güncellemesi için OnOrderReady'yi tetikler.
	void Setup(ACoffeeShopPlayerController* InOwningController,
		ACoffeeShopCustomerServicePoint* InServicePoint,
		ACoffeeShopCustomerCharacter* InCustomer);

	// "ONAYLA" butonu: ödemeyi kabul eder (müşteri pickup'a yürür) ve ekranı kapatır.
	UFUNCTION(BlueprintCallable, Category = "Order Confirm")
	void ConfirmOrder();

	// "KAPAT" butonu: hiçbir şey yapmadan ekranı kapatır; müşteri kasada bekler.
	UFUNCTION(BlueprintCallable, Category = "Order Confirm")
	void CloseOrder();

protected:
	// Bağlı müşteri (sıradaki ödeme bekleyen).
	UPROPERTY(BlueprintReadOnly, Category = "Order Confirm")
	TObjectPtr<ACoffeeShopCustomerCharacter> Customer;

	// Müşterinin siparişi (DrinkId + DisplayName). WBP bunu metne basar.
	UPROPERTY(BlueprintReadOnly, Category = "Order Confirm")
	FCoffeeShopCustomerOrderRequest Order;

	// WBP bu event'te Text/Image gibi görselleri Order'a göre günceller.
	UFUNCTION(BlueprintImplementableEvent, Category = "Order Confirm")
	void OnOrderReady();

private:
	UPROPERTY()
	TObjectPtr<ACoffeeShopCustomerServicePoint> ServicePoint;

	UPROPERTY()
	TObjectPtr<ACoffeeShopPlayerController> OwningController;

	// Ekranı viewport'tan kaldırır ve oyunu Game input moduna döndürür.
	void DismissAndRestoreGameInput();
};
