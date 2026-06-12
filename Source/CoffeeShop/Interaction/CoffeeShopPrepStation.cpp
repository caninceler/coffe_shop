// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CoffeeShopPrepStation.h"
#include "Player/CoffeeShopPlayerController.h"
#include "Components/SceneComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Pawn.h"

ACoffeeShopPrepStation::ACoffeeShopPrepStation()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

void ACoffeeShopPrepStation::Interact_Implementation(AActor* Interactor)
{
	ACoffeeShopPlayerController* PlayerController = nullptr;
	if (const APawn* InteractorPawn = Cast<APawn>(Interactor))
	{
		PlayerController = Cast<ACoffeeShopPlayerController>(InteractorPawn->GetController());
	}
	if (!PlayerController)
	{
		return;
	}

	// Tarifte sıradaki adım bu istasyonunkiyse ilerlet; değilse uyarı (PlayerController içinde).
	PlayerController->TryPrepStep(ProvidedStep);
}

FText ACoffeeShopPrepStation::GetInteractionPrompt_Implementation() const
{
	return InteractionPrompt.IsEmpty() ? FText::FromString(TEXT("Hazırla")) : InteractionPrompt;
}

void ACoffeeShopPrepStation::OnInteractionFocusChanged_Implementation(bool bFocused)
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
