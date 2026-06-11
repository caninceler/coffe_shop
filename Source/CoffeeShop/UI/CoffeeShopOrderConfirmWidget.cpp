// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/CoffeeShopOrderConfirmWidget.h"
#include "Customer/CoffeeShopCustomerCharacter.h"
#include "Customer/CoffeeShopCustomerServicePoint.h"
#include "Player/CoffeeShopPlayerController.h"

void UCoffeeShopOrderConfirmWidget::Setup(ACoffeeShopPlayerController* InOwningController,
	ACoffeeShopCustomerServicePoint* InServicePoint,
	ACoffeeShopCustomerCharacter* InCustomer)
{
	OwningController = InOwningController;
	ServicePoint = InServicePoint;
	Customer = InCustomer;
	Order = IsValid(InCustomer) ? InCustomer->OrderRequest : FCoffeeShopCustomerOrderRequest();

	// WBP görselleri sipariş verisine göre güncellesin.
	OnOrderReady();
}

void UCoffeeShopOrderConfirmWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Ekran açıkken sıradaki müşteriyi yeniden sorgula. Kasaya ulaşmış bir müşteri varsa
	// onu göster, yoksa null. Sadece DEĞİŞTİĞİNDE görselleri güncelle (her frame değil).
	ACoffeeShopCustomerCharacter* CurrentCustomer = nullptr;
	if (IsValid(ServicePoint) && ServicePoint->IsNextPaymentCustomerAtCounter())
	{
		CurrentCustomer = ServicePoint->GetNextPaymentCustomer();
	}

	if (CurrentCustomer != Customer)
	{
		Customer = CurrentCustomer;
		Order = IsValid(Customer) ? Customer->OrderRequest : FCoffeeShopCustomerOrderRequest();
		OnOrderReady();
	}
}

void UCoffeeShopOrderConfirmWidget::ConfirmOrder()
{
	// Müşteri yoksa onaylanacak sipariş de yok.
	if (!IsValid(Customer))
	{
		return;
	}

	if (IsValid(ServicePoint))
	{
		// Mevcut ödeme akışı: kabul edilirse müşteri pickup noktasına yürür.
		ServicePoint->AcceptNextPayment();
	}

	DismissAndRestoreGameInput();
}

bool UCoffeeShopOrderConfirmWidget::HasCustomer() const
{
	return IsValid(Customer);
}

void UCoffeeShopOrderConfirmWidget::CloseOrder()
{
	// Sipariş alınmadan kapat; müşteri kasada beklemeye devam eder.
	DismissAndRestoreGameInput();
}

void UCoffeeShopOrderConfirmWidget::DismissAndRestoreGameInput()
{
	RemoveFromParent();

	if (IsValid(OwningController))
	{
		OwningController->CloseOrderConfirm();
	}
}
