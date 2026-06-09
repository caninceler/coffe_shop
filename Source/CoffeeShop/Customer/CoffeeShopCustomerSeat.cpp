// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoffeeShopCustomerSeat.h"
#include "Components/SceneComponent.h"

ACoffeeShopCustomerSeat::ACoffeeShopCustomerSeat()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SitPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SitPoint"));
	SitPoint->SetupAttachment(Root);
}

bool ACoffeeShopCustomerSeat::ClaimSeat(AActor* Customer)
{
	if (!Customer || OccupyingCustomer)
	{
		return false;
	}

	OccupyingCustomer = Customer;
	return true;
}

void ACoffeeShopCustomerSeat::ReleaseSeat(AActor* Customer)
{
	if (Customer && OccupyingCustomer == Customer)
	{
		OccupyingCustomer = nullptr;
	}
}

bool ACoffeeShopCustomerSeat::IsAvailable() const
{
	return OccupyingCustomer == nullptr;
}

FTransform ACoffeeShopCustomerSeat::GetSitTransform() const
{
	return SitPoint ? SitPoint->GetComponentTransform() : GetActorTransform();
}
