// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CoffeeShopPaymentTerminal.h"
#include "Customer/CoffeeShopCustomerServicePoint.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"

ACoffeeShopPaymentTerminal::ACoffeeShopPaymentTerminal()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

void ACoffeeShopPaymentTerminal::Interact_Implementation(AActor* Interactor)
{
	if (!ServicePoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("Payment terminal %s has no ServicePoint."), *GetNameSafe(this));
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("%s used payment terminal %s."), *GetNameSafe(Interactor), *GetNameSafe(this));
	ServicePoint->AcceptNextPayment();
}

FText ACoffeeShopPaymentTerminal::GetInteractionPrompt_Implementation() const
{
	return FText::FromString(TEXT("Take Order"));
}

void ACoffeeShopPaymentTerminal::OnInteractionFocusChanged_Implementation(bool bFocused)
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
