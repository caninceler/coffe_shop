// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoffeeShopCustomerSpawnPoint.h"
#include "CoffeeShopCustomerCharacter.h"
#include "CoffeeShopCustomerServicePoint.h"
#include "Components/SceneComponent.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "Engine/World.h"

ACoffeeShopCustomerSpawnPoint::ACoffeeShopCustomerSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

void ACoffeeShopCustomerSpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoSpawn)
	{
		ScheduleNextAutoSpawn();
	}
}

ACoffeeShopCustomerCharacter* ACoffeeShopCustomerSpawnPoint::SpawnCustomer()
{
	if (!CustomerClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnCustomer failed on %s: CustomerClass is null."), *GetNameSafe(this));
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnCustomer failed on %s: World is null."), *GetNameSafe(this));
		return nullptr;
	}

	ACoffeeShopCustomerServicePoint* ServicePoint = DefaultServicePoint;
	if (!ServicePoint)
	{
		for (TActorIterator<ACoffeeShopCustomerServicePoint> It(World); It; ++It)
		{
			ServicePoint = *It;
			UE_LOG(LogTemp, Warning, TEXT("SpawnCustomer on %s recovered missing DefaultServicePoint from world: %s."), *GetNameSafe(this), *GetNameSafe(ServicePoint));
			break;
		}
	}

	if (ServicePoint && !ServicePoint->CanAcceptNewCustomer())
	{
		UE_LOG(LogTemp, Display, TEXT("SpawnCustomer skipped on %s: service flow is full at %s."), *GetNameSafe(this), *GetNameSafe(ServicePoint));
		return nullptr;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ACoffeeShopCustomerCharacter* Customer = World->SpawnActor<ACoffeeShopCustomerCharacter>(
		CustomerClass,
		GetActorTransform(),
		SpawnParameters);

	if (Customer)
	{
		Customer->SetServicePoint(ServicePoint);
		// İlk versiyon: her müşteri "Americano" ister. (Sonra rastgele menü seçimine çevrilebilir.)
		Customer->OrderRequest.DrinkId = TEXT("Americano");
		Customer->OrderRequest.DisplayName = FText::FromString(TEXT("Americano"));
		// Çoğu müşteri oturmak ister (SitDownChance); azı take away. Oturmak isteyen
		// boş masa bulamazsa karar düğümünde yine take away'e döner.
		Customer->SetWantsToSitDown(FMath::FRand() < SitDownChance);
		Customer->SetVisitState(ECoffeeShopCustomerVisitState::Entering);
		UE_LOG(LogTemp, Display, TEXT("Spawned customer %s with ServicePoint %s."), *GetNameSafe(Customer), *GetNameSafe(ServicePoint));
	}

	return Customer;
}

void ACoffeeShopCustomerSpawnPoint::SpawnCustomerAndScheduleNext()
{
	SpawnCustomer();
	ScheduleNextAutoSpawn();
}

void ACoffeeShopCustomerSpawnPoint::ScheduleNextAutoSpawn()
{
	UWorld* World = GetWorld();
	if (!World || !bAutoSpawn)
	{
		return;
	}

	const float MinDelay = FMath::Max(0.1f, FMath::Min(MinAutoSpawnDelay, MaxAutoSpawnDelay));
	const float MaxDelay = FMath::Max(MinDelay, FMath::Max(MinAutoSpawnDelay, MaxAutoSpawnDelay));
	const float NextDelay = FMath::FRandRange(MinDelay, MaxDelay);

	World->GetTimerManager().SetTimer(
		AutoSpawnTimerHandle,
		this,
		&ACoffeeShopCustomerSpawnPoint::SpawnCustomerAndScheduleNext,
		NextDelay,
		false);
}
