// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CoffeeShopPaymentTerminal.h"
#include "Customer/CoffeeShopCustomerServicePoint.h"
#include "Player/CoffeeShopPlayerController.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Pawn.h"

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

	// Eskiden doğrudan AcceptNextPayment çağrılırdı. Artık önce sipariş onay ekranı
	// açılır; ödeme yalnızca oyuncu ekranda "Onayla" deyince kabul edilir.
	ACoffeeShopPlayerController* PlayerController = nullptr;
	if (const APawn* InteractorPawn = Cast<APawn>(Interactor))
	{
		PlayerController = Cast<ACoffeeShopPlayerController>(InteractorPawn->GetController());
	}

	if (PlayerController)
	{
		// Kamera hedefi olarak terminalin kendisini geçiyoruz (Camera Component'i varsa
		// onun açısı kullanılır). Asıl POS sahnede genelde BP_POS_Touchscreen'dir ve
		// OpenOrderConfirm'i kendisi çağırır; bu C++ yolu yedek/test içindir.
		PlayerController->OpenOrderConfirm(ServicePoint, this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Payment terminal %s could not resolve a CoffeeShop player controller from %s."),
			*GetNameSafe(this),
			*GetNameSafe(Interactor));
	}
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
