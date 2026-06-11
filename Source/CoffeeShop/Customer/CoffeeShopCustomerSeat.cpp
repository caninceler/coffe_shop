// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoffeeShopCustomerSeat.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"

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

	// Başlangıçtaki aktiflik durumuna göre modelleri göster/gizle.
	ApplyActiveVisual();
}

void ACoffeeShopCustomerSeat::ApplyActiveVisual()
{
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());

	for (const TObjectPtr<AActor>& Model : GroupModels)
	{
		if (!IsValid(Model))
		{
			continue;
		}

		Model->SetActorHiddenInGame(!bActive);
		Model->SetActorEnableCollision(bActive);

		if (!NavSystem)
		{
			continue;
		}

		// Model gizlenince/görününce o alandaki NavMesh'i yeniden hesaplat ki müşteriler
		// boşalan yerden yürüyebilsin (RuntimeGeneration = Dynamic). Aktörün her primitive
		// (çarpışmalı) component'ini octree'de güncelleyip o bölgeyi "dirty" işaretliyoruz.
		TArray<UPrimitiveComponent*> PrimitiveComponents;
		Model->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
		for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
		{
			if (PrimitiveComponent)
			{
				NavSystem->UpdateComponentInNavOctree(*PrimitiveComponent);
			}
		}
		NavSystem->UpdateActorInNavOctree(*Model);
	}
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
	// Aktiflik değişince bağlı modelleri de göster/gizle.
	ApplyActiveVisual();
}

bool ACoffeeShopCustomerSeat::IsSeatActive() const
{
	return bActive;
}

int32 ACoffeeShopCustomerSeat::GetGroupId() const
{
	return GroupId;
}

bool ACoffeeShopCustomerSeat::SetSeatGroupActive(const UObject* WorldContextObject, int32 GroupId, bool bNewActive)
{
	const UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) : nullptr;
	if (!World)
	{
		return false;
	}

	// Oturan müşteri varken grubu KAPATMA — müşteri havada kalır.
	if (!bNewActive && IsSeatGroupOccupied(WorldContextObject, GroupId))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetSeatGroupActive: group %d cannot be closed; a customer is seated."), GroupId);
		return false;
	}

	int32 AffectedCount = 0;
	for (TActorIterator<ACoffeeShopCustomerSeat> It(World); It; ++It)
	{
		ACoffeeShopCustomerSeat* Seat = *It;
		if (IsValid(Seat) && Seat->GetGroupId() == GroupId)
		{
			Seat->SetSeatActive(bNewActive);
			++AffectedCount;
		}
	}

	UE_LOG(LogTemp, Display, TEXT("SetSeatGroupActive: group %d set to %s (%d seats)."),
		GroupId, bNewActive ? TEXT("active") : TEXT("inactive"), AffectedCount);
	return true;
}

bool ACoffeeShopCustomerSeat::IsSeatGroupOccupied(const UObject* WorldContextObject, int32 GroupId)
{
	const UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) : nullptr;
	if (!World)
	{
		return false;
	}

	for (TActorIterator<ACoffeeShopCustomerSeat> It(World); It; ++It)
	{
		const ACoffeeShopCustomerSeat* Seat = *It;
		if (IsValid(Seat) && Seat->GetGroupId() == GroupId && Seat->IsOccupied())
		{
			return true;
		}
	}

	return false;
}

bool ACoffeeShopCustomerSeat::IsOccupied() const
{
	return OccupyingCustomer != nullptr;
}

bool ACoffeeShopCustomerSeat::IsSeatGroupActive(const UObject* WorldContextObject, int32 GroupId)
{
	const UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) : nullptr;
	if (!World)
	{
		return false;
	}

	for (TActorIterator<ACoffeeShopCustomerSeat> It(World); It; ++It)
	{
		const ACoffeeShopCustomerSeat* Seat = *It;
		if (IsValid(Seat) && Seat->GetGroupId() == GroupId && Seat->IsSeatActive())
		{
			return true;
		}
	}

	return false;
}
