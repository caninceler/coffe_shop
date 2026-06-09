// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/CoffeeShopPlayerController.h"
#include "Components/PrimitiveComponent.h"
#include "Interaction/CoffeeShopInteractable.h"

ACoffeeShopPlayerController::ACoffeeShopPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACoffeeShopPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	UpdateFocusedInteractable();
}

AActor* ACoffeeShopPlayerController::GetFocusedInteractable() const
{
	return FocusedInteractable;
}

void ACoffeeShopPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &ACoffeeShopPlayerController::Interact);
	InputComponent->BindAction(TEXT("Interact_Debug"), IE_Pressed, this, &ACoffeeShopPlayerController::Interact);
	UE_LOG(LogTemp, Display, TEXT("CoffeeShop player controller input bindings installed."));
}

void ACoffeeShopPlayerController::Interact()
{
	UE_LOG(LogTemp, Display, TEXT("CoffeeShop controller interaction pressed."));

	UpdateFocusedInteractable();
	if (FocusedInteractable)
	{
		ICoffeeShopInteractable::Execute_Interact(FocusedInteractable, GetPawn());
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("CoffeeShop controller interaction found no interactable target."));
}

void ACoffeeShopPlayerController::UpdateFocusedInteractable()
{
	FVector ViewLocation;
	FRotator ViewRotation;
	GetPlayerViewPoint(ViewLocation, ViewRotation);

	FHitResult Hit;
	const FVector End = ViewLocation + ViewRotation.Vector() * InteractionDistance;
	if (GetWorld()->LineTraceSingleByChannel(Hit, ViewLocation, End, ECC_Visibility))
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->GetClass()->ImplementsInterface(UCoffeeShopInteractable::StaticClass()))
		{
			SetFocusedInteractable(HitActor);
			return;
		}
	}

	SetFocusedInteractable(nullptr);
}

void ACoffeeShopPlayerController::SetFocusedInteractable(AActor* NewFocusedActor)
{
	if (FocusedInteractable == NewFocusedActor)
	{
		return;
	}

	SetActorHighlight(FocusedInteractable, false);
	if (FocusedInteractable && FocusedInteractable->GetClass()->ImplementsInterface(UCoffeeShopInteractable::StaticClass()))
	{
		ICoffeeShopInteractable::Execute_OnInteractionFocusChanged(FocusedInteractable, false);
	}

	FocusedInteractable = NewFocusedActor;

	SetActorHighlight(FocusedInteractable, true);
	if (FocusedInteractable && FocusedInteractable->GetClass()->ImplementsInterface(UCoffeeShopInteractable::StaticClass()))
	{
		ICoffeeShopInteractable::Execute_OnInteractionFocusChanged(FocusedInteractable, true);
	}
}

void ACoffeeShopPlayerController::SetActorHighlight(AActor* Actor, bool bHighlighted) const
{
	if (!Actor)
	{
		return;
	}

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	Actor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
	for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
	{
		if (!PrimitiveComponent)
		{
			continue;
		}

		PrimitiveComponent->SetRenderCustomDepth(bHighlighted);
		PrimitiveComponent->SetCustomDepthStencilValue(bHighlighted ? FocusStencilValue : 0);
	}
}
