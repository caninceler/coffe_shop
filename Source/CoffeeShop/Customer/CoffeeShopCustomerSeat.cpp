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

void ACoffeeShopCustomerSeat::BeginPlay()
{
	Super::BeginPlay();

	// Boş dükkan: her koltuk pasif başlar. Sandalye + masa yerleştirilince yalnızca
	// TableArea bunu açar. Editörde yanlışlıkla Active=true bırakılmış olsa bile,
	// burada pasifliyoruz ki modeli görünmeyen bir koltuğa müşteri oturmasın.
	bActive = false;
}

bool ACoffeeShopCustomerSeat::ClaimSeat(AActor* Customer)
{
	if (!Customer || OccupyingCustomer || !bActive)
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
	// Pasif koltuk asla müsait değildir; aktif ve boşsa müsaittir.
	return bActive && OccupyingCustomer == nullptr;
}

FTransform ACoffeeShopCustomerSeat::GetSitTransform() const
{
	return SitPoint ? SitPoint->GetComponentTransform() : GetActorTransform();
}

void ACoffeeShopCustomerSeat::SetSeatActive(bool bNewActive)
{
	bActive = bNewActive;
}

bool ACoffeeShopCustomerSeat::IsSeatActive() const
{
	return bActive;
}

bool ACoffeeShopCustomerSeat::IsOccupied() const
{
	return OccupyingCustomer != nullptr;
}
