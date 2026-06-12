// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoffeeShopCustomerServicePoint.h"
#include "CoffeeShopCustomerAIController.h"
#include "CoffeeShopCustomerCharacter.h"
#include "Components/SceneComponent.h"

ACoffeeShopCustomerServicePoint::ACoffeeShopCustomerServicePoint()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	QueuePoint = CreateDefaultSubobject<USceneComponent>(TEXT("QueuePoint"));
	QueuePoint->SetupAttachment(Root);

	PaymentPoint = CreateDefaultSubobject<USceneComponent>(TEXT("PaymentPoint"));
	PaymentPoint->SetupAttachment(Root);

	PickupPoint = CreateDefaultSubobject<USceneComponent>(TEXT("PickupPoint"));
	PickupPoint->SetupAttachment(Root);
}

FTransform ACoffeeShopCustomerServicePoint::GetQueueTransform() const
{
	return QueuePoint ? QueuePoint->GetComponentTransform() : GetActorTransform();
}

FTransform ACoffeeShopCustomerServicePoint::GetPaymentTransform() const
{
	return PaymentPoint ? PaymentPoint->GetComponentTransform() : GetActorTransform();
}

FVector ACoffeeShopCustomerServicePoint::GetPaymentQueueLocationForCustomer(ACoffeeShopCustomerCharacter* Customer)
{
	RegisterWaitingForPayment(Customer);

	const FTransform PaymentTransform = GetPaymentTransform();
	const int32 QueueIndex = PaymentQueue.IndexOfByKey(Customer);
	const int32 SafeQueueIndex = QueueIndex == INDEX_NONE ? 0 : QueueIndex;
	return PaymentTransform.GetLocation() - PaymentTransform.GetRotation().GetForwardVector() * PaymentQueueSlotSpacing * SafeQueueIndex;
}

FTransform ACoffeeShopCustomerServicePoint::GetPickupTransform() const
{
	return PickupPoint ? PickupPoint->GetComponentTransform() : GetActorTransform();
}

FTransform ACoffeeShopCustomerServicePoint::GetOrderTransform() const
{
	return GetPaymentTransform();
}

void ACoffeeShopCustomerServicePoint::RegisterWaitingForPayment(ACoffeeShopCustomerCharacter* Customer)
{
	CompactQueues();

	if (!Customer)
	{
		return;
	}

	if (Customer->IsPaymentAccepted())
	{
		return;
	}

	const ECoffeeShopCustomerVisitState VisitState = Customer->GetVisitState();
	if (VisitState != ECoffeeShopCustomerVisitState::Entering
		&& VisitState != ECoffeeShopCustomerVisitState::WalkingToCounter
		&& VisitState != ECoffeeShopCustomerVisitState::Ordering)
	{
		return;
	}

	PaymentQueue.AddUnique(Customer);
	UE_LOG(LogTemp, Display, TEXT("%s registered for payment at %s. Queue size: %d"),
		*GetNameSafe(Customer),
		*GetNameSafe(this),
		PaymentQueue.Num());
}

bool ACoffeeShopCustomerServicePoint::AcceptNextPayment()
{
	CompactQueues();

	if (IsValid(ActivePickupCustomer))
	{
		UE_LOG(LogTemp, Warning, TEXT("AcceptNextPayment blocked on %s: pickup point is occupied by %s."),
			*GetNameSafe(this),
			*GetNameSafe(ActivePickupCustomer));
		return false;
	}

	if (PaymentQueue.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("AcceptNextPayment failed on %s: payment queue is empty."), *GetNameSafe(this));
		return false;
	}

	ACoffeeShopCustomerCharacter* Customer = PaymentQueue[0];

	// Müşteri henüz kasaya yürüyor olabilir; ödeme noktasının önüne ulaşmadan
	// siparişi alma. Sadece yeterince yaklaştığında "Take Order" işler.
	if (IsValid(Customer) && !HasCustomerReachedCounter(Customer))
	{
		UE_LOG(LogTemp, Display, TEXT("AcceptNextPayment blocked on %s: %s has not reached the counter yet."),
			*GetNameSafe(this),
			*GetNameSafe(Customer));
		return false;
	}

	PaymentQueue.RemoveAt(0);
	ActivePickupCustomer = Customer;
	Customer->AcceptPayment();
	if (ACoffeeShopCustomerAIController* CustomerController = Cast<ACoffeeShopCustomerAIController>(Customer->GetController()))
	{
		CustomerController->RestartCustomerBehavior();
	}
	RefreshPaymentQueuePositions();
	UE_LOG(LogTemp, Display, TEXT("Accepted payment for %s at %s. Queue size: %d"),
		*GetNameSafe(Customer),
		*GetNameSafe(this),
		PaymentQueue.Num());
	return true;
}

ACoffeeShopCustomerCharacter* ACoffeeShopCustomerServicePoint::GetNextPaymentCustomer() const
{
	for (const TObjectPtr<ACoffeeShopCustomerCharacter>& Customer : PaymentQueue)
	{
		if (IsValid(Customer))
		{
			return Customer;
		}
	}

	return nullptr;
}

bool ACoffeeShopCustomerServicePoint::IsNextPaymentCustomerAtCounter() const
{
	const ACoffeeShopCustomerCharacter* Customer = GetNextPaymentCustomer();
	return Customer != nullptr && HasCustomerReachedCounter(Customer);
}

bool ACoffeeShopCustomerServicePoint::HasCustomerReachedCounter(const ACoffeeShopCustomerCharacter* Customer) const
{
	if (!IsValid(Customer))
	{
		return false;
	}

	const float DistanceToPayment = FVector::Dist(Customer->GetActorLocation(), GetPaymentTransform().GetLocation());
	return DistanceToPayment <= PaymentReachDistance;
}

void ACoffeeShopCustomerServicePoint::RegisterWaitingForDrink(ACoffeeShopCustomerCharacter* Customer)
{
	CompactQueues();

	if (!Customer)
	{
		return;
	}

	ActivePickupCustomer = Customer;
	PickupQueue.AddUnique(Customer);
	UE_LOG(LogTemp, Display, TEXT("%s registered for drink pickup at %s. Pickup queue size: %d"),
		*GetNameSafe(Customer),
		*GetNameSafe(this),
		PickupQueue.Num());
}

ACoffeeShopCustomerCharacter* ACoffeeShopCustomerServicePoint::GetNextDrinkCustomer() const
{
	for (const TObjectPtr<ACoffeeShopCustomerCharacter>& Customer : PickupQueue)
	{
		if (IsValid(Customer))
		{
			return Customer;
		}
	}
	return nullptr;
}

bool ACoffeeShopCustomerServicePoint::ServeNextDrink()
{
	CompactQueues();

	if (PickupQueue.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("ServeNextDrink failed on %s: pickup queue is empty."), *GetNameSafe(this));
		return false;
	}

	ACoffeeShopCustomerCharacter* Customer = PickupQueue[0];
	PickupQueue.RemoveAt(0);
	if (ActivePickupCustomer == Customer)
	{
		ActivePickupCustomer = nullptr;
	}
	RefreshPaymentQueuePositions();
	Customer->MarkOrderServed();
	UE_LOG(LogTemp, Display, TEXT("Served drink for %s at %s. Pickup queue size: %d"),
		*GetNameSafe(Customer),
		*GetNameSafe(this),
		PickupQueue.Num());
	return true;
}

void ACoffeeShopCustomerServicePoint::RemoveCustomerFromQueues(ACoffeeShopCustomerCharacter* Customer)
{
	if (!Customer)
	{
		return;
	}

	PaymentQueue.Remove(Customer);
	PickupQueue.Remove(Customer);
	if (ActivePickupCustomer == Customer)
	{
		ActivePickupCustomer = nullptr;
	}

	RefreshPaymentQueuePositions();
}

bool ACoffeeShopCustomerServicePoint::IsFirstWaitingForPayment(ACoffeeShopCustomerCharacter* Customer) const
{
	return Customer && PaymentQueue.Num() > 0 && PaymentQueue[0] == Customer;
}

bool ACoffeeShopCustomerServicePoint::CanAcceptNewCustomer() const
{
	int32 ActiveCustomerCount = 0;

	for (const TObjectPtr<ACoffeeShopCustomerCharacter>& Customer : PaymentQueue)
	{
		if (IsValid(Customer))
		{
			++ActiveCustomerCount;
		}
	}

	for (const TObjectPtr<ACoffeeShopCustomerCharacter>& Customer : PickupQueue)
	{
		if (IsValid(Customer))
		{
			++ActiveCustomerCount;
		}
	}

	if (IsValid(ActivePickupCustomer) && !PickupQueue.Contains(ActivePickupCustomer))
	{
		++ActiveCustomerCount;
	}

	return ActiveCustomerCount < MaxCustomersInServiceFlow;
}


void ACoffeeShopCustomerServicePoint::CompactQueues()
{
	PaymentQueue.RemoveAll([](const TObjectPtr<ACoffeeShopCustomerCharacter>& Customer)
	{
		return !IsValid(Customer);
	});

	PickupQueue.RemoveAll([](const TObjectPtr<ACoffeeShopCustomerCharacter>& Customer)
	{
		return !IsValid(Customer);
	});

	if (!IsValid(ActivePickupCustomer))
	{
		ActivePickupCustomer = nullptr;
	}
}

void ACoffeeShopCustomerServicePoint::RefreshPaymentQueuePositions()
{
	CompactQueues();

	TArray<TObjectPtr<ACoffeeShopCustomerCharacter>> CustomersToRestart;
	for (const TObjectPtr<ACoffeeShopCustomerCharacter>& Customer : PaymentQueue)
	{
		if (!IsValid(Customer))
		{
			continue;
		}

		const ECoffeeShopCustomerVisitState VisitState = Customer->GetVisitState();
		if (VisitState == ECoffeeShopCustomerVisitState::Leaving
			|| VisitState == ECoffeeShopCustomerVisitState::Angry
			|| VisitState == ECoffeeShopCustomerVisitState::WaitingForDrink
			|| VisitState == ECoffeeShopCustomerVisitState::LookingForSeat
			|| VisitState == ECoffeeShopCustomerVisitState::WalkingToSeat
			|| VisitState == ECoffeeShopCustomerVisitState::Seated)
		{
			continue;
		}

		if (VisitState == ECoffeeShopCustomerVisitState::Ordering)
		{
			Customer->SetVisitState(ECoffeeShopCustomerVisitState::WalkingToCounter);
		}

		CustomersToRestart.Add(Customer);
	}

	for (const TObjectPtr<ACoffeeShopCustomerCharacter>& Customer : CustomersToRestart)
	{
		if (IsValid(Customer))
		{
			if (ACoffeeShopCustomerAIController* CustomerController = Cast<ACoffeeShopCustomerAIController>(Customer->GetController()))
			{
				CustomerController->RestartCustomerBehavior();
			}
		}
	}
}
