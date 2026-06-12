// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CoffeeShopPickupCounter.h"
#include "Customer/CoffeeShopCustomerServicePoint.h"
#include "Player/CoffeeShopPlayerController.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

ACoffeeShopPickupCounter::ACoffeeShopPickupCounter()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// Bardağın duracağı nokta (editörde tezgah üstüne konumlandırılır).
	CupPoint = CreateDefaultSubobject<USceneComponent>(TEXT("CupPoint"));
	CupPoint->SetupAttachment(Root);
}

void ACoffeeShopPickupCounter::Interact_Implementation(AActor* Interactor)
{
	if (!ServicePoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pickup counter %s has no ServicePoint."), *GetNameSafe(this));
		return;
	}

	// Tezgahta zaten bekleyen bir bardak varsa yenisini koyma.
	if (IsValid(WaitingCup))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(21, 2.0f, FColor::Orange, TEXT("Tezgahta zaten bardak var."));
		}
		return;
	}

	// Oyuncunun elinde HAZIR kahve olmalı.
	ACoffeeShopPlayerController* PlayerController = nullptr;
	if (const APawn* InteractorPawn = Cast<APawn>(Interactor))
	{
		PlayerController = Cast<ACoffeeShopPlayerController>(InteractorPawn->GetController());
	}

	if (!PlayerController || !PlayerController->IsDrinkReady())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(21, 2.0f, FColor::Orange, TEXT("Önce kahveyi hazırla!"));
		}
		return;
	}

	// Bardağı tezgaha bırak (görünür durur).
	const FTransform DropTransform = CupPoint ? CupPoint->GetComponentTransform() : GetActorTransform();
	WaitingCup = PlayerController->PlaceCupAt(DropTransform);
	if (!IsValid(WaitingCup))
	{
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("%s placed cup at %s."), *GetNameSafe(Interactor), *GetNameSafe(this));

	// Pickup'ta bekleyen müşteri varsa kısa süre sonra alınsın; yoksa bardak bekler
	// (müşteri gelince yine bu counter'a uğrayacak — şimdilik: bekleyen varsa al).
	if (ServicePoint->GetNextDrinkCustomer())
	{
		GetWorldTimerManager().SetTimer(PickupTimerHandle, this,
			&ACoffeeShopPickupCounter::CompletePickup, FMath::Max(0.01f, PickupDelaySeconds), false);
	}
}

void ACoffeeShopPickupCounter::CompletePickup()
{
	if (!ServicePoint)
	{
		return;
	}

	// NPC bardağı alır: serve + bardağı yok et.
	if (ServicePoint->ServeNextDrink())
	{
		if (IsValid(WaitingCup))
		{
			WaitingCup->Destroy();
		}
		WaitingCup = nullptr;
		UE_LOG(LogTemp, Display, TEXT("Pickup %s: customer took the cup."), *GetNameSafe(this));
	}
}

FText ACoffeeShopPickupCounter::GetInteractionPrompt_Implementation() const
{
	return IsValid(WaitingCup) ? FText::GetEmpty() : FText::FromString(TEXT("Kahveyi Bırak"));
}

void ACoffeeShopPickupCounter::OnInteractionFocusChanged_Implementation(bool bFocused)
{
	TArray<UPrimitiveComponent*> PrimitiveComponents;
	GetComponents<UPrimitiveComponent>(PrimitiveComponents);
	for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
	{
		if (PrimitiveComponent)
		{
			PrimitiveComponent->SetRenderCustomDepth(bFocused);
		}
	}
}
