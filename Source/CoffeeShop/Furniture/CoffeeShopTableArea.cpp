// Copyright Epic Games, Inc. All Rights Reserved.

#include "Furniture/CoffeeShopTableArea.h"
#include "Furniture/CoffeeShopFurnitureSlot.h"
#include "Customer/CoffeeShopCustomerSeat.h"
#include "Components/SceneComponent.h"

ACoffeeShopTableArea::ACoffeeShopTableArea()
{
	PrimaryActorTick.bCanEverTick = true;
	// Her frame şart değil; saniyede birkaç kez yeterli.
	PrimaryActorTick.TickInterval = 0.25f;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

void ACoffeeShopTableArea::BeginPlay()
{
	Super::BeginPlay();

	// Teşhis: referanslar doğru bağlanmış mı?
	UE_LOG(LogTemp, Warning, TEXT("TableArea %s: TableSlot=%s, ChairSlots=%d, SeatPoints=%d"),
		*GetNameSafe(this),
		*GetNameSafe(TableSlot),
		ChairSlots.Num(),
		SeatPoints.Num());

	RefreshSeats();
}

void ACoffeeShopTableArea::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	RefreshSeats();
}

void ACoffeeShopTableArea::RefreshSeats()
{
	// Masa yerleşmiş mi?
	const bool bTablePlaced = IsValid(TableSlot) && TableSlot->IsOccupied();

	for (int32 Index = 0; Index < SeatPoints.Num(); ++Index)
	{
		ACoffeeShopCustomerSeat* Seat = SeatPoints[Index];
		if (!IsValid(Seat))
		{
			continue;
		}

		// Bu sandalye slotu dolu mu?
		const bool bChairPlaced = ChairSlots.IsValidIndex(Index)
			&& IsValid(ChairSlots[Index])
			&& ChairSlots[Index]->IsOccupied();

		// Oturulabilir: masa var VE bu sandalye yerleşmiş.
		const bool bShouldBeActive = bTablePlaced && bChairPlaced;

		// Sadece değiştiğinde uygula (gereksiz NavMesh/görsel güncellemesi olmasın).
		if (Seat->IsSeatActive() != bShouldBeActive)
		{
			Seat->SetSeatActive(bShouldBeActive);
			UE_LOG(LogTemp, Warning, TEXT("TableArea %s: seat %s set active=%d (tablePlaced=%d, chairPlaced=%d)"),
				*GetNameSafe(this), *GetNameSafe(Seat), bShouldBeActive ? 1 : 0,
				bTablePlaced ? 1 : 0, bChairPlaced ? 1 : 0);
		}
	}
}
