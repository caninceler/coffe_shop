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

void UCoffeeShopOrderConfirmWidget::ConfirmOrder()
{
	if (IsValid(ServicePoint))
	{
		// Mevcut ödeme akışı: kabul edilirse müşteri pickup noktasına yürür.
		ServicePoint->AcceptNextPayment();
	}

	DismissAndRestoreGameInput();
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
