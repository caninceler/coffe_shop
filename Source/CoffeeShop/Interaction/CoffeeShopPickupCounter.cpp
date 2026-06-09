// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CoffeeShopPickupCounter.h"
#include "Customer/CoffeeShopCustomerServicePoint.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"

ACoffeeShopPickupCounter::ACoffeeShopPickupCounter()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

void ACoffeeShopPickupCounter::Interact_Implementation(AActor* Interactor)
{
	if (!ServicePoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pickup counter %s has no ServicePoint."), *GetNameSafe(this));
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("%s used pickup counter %s."), *GetNameSafe(Interactor), *GetNameSafe(this));
	ServicePoint->ServeNextDrink();
}

FText ACoffeeShopPickupCounter::GetInteractionPrompt_Implementation() const
{
	return FText::FromString(TEXT("Serve Coffee"));
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
