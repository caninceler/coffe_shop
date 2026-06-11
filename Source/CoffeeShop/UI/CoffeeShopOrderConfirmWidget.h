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
	// Müşteri yoksa hiçbir şey yapmaz.
	UFUNCTION(BlueprintCallable, Category = "Order Confirm")
	void ConfirmOrder();

	// "KAPAT" butonu: hiçbir şey yapmadan ekranı kapatır; müşteri kasada bekler.
	UFUNCTION(BlueprintCallable, Category = "Order Confirm")
	void CloseOrder();

	// Sol panel için: gösterilecek bir müşteri/sipariş var mı?
	UFUNCTION(BlueprintPure, Category = "Order Confirm")
	bool HasCustomer() const;

	// Ekran açıkken müşteri durumunu yeniden kontrol eder. Sıradaki müşteri değiştiyse
	// (sonradan geldi/gitti) Customer + Order güncellenir ve OnOrderReady tetiklenir,
	// böylece WBP "Müşteri yok" → sipariş arasında canlı geçiş yapar.
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// --- Sağ panel: Masa/sandalye yönetimi ---

	// Bir masa grubunu aç/kapa (toggle). WBP'deki 7 butondan biri çağırır (GroupId 0..6).
	UFUNCTION(BlueprintCallable, Category = "Table Management")
	void ToggleTableGroup(int32 GroupId);

	// Bir masa grubu şu an aktif mi? Buton görünümünü (Açık/Kapalı) güncellemek için.
	UFUNCTION(BlueprintPure, Category = "Table Management")
	bool IsTableGroupActive(int32 GroupId) const;

	// Grupta oturan müşteri var mı? (Doluysa kapatılamaz — buton kilitli gösterilebilir.)
	UFUNCTION(BlueprintPure, Category = "Table Management")
	bool IsTableGroupOccupied(int32 GroupId) const;

	// WBP'nin masa butonlarını (Açık/Kapalı/Dolu) güncellemesi için event. Toggle
	// sonrası ve ekran açılışında çağrılır.
	UFUNCTION(BlueprintImplementableEvent, Category = "Table Management")
	void OnTableGroupsChanged();

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
