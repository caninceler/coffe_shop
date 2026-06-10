// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/CoffeeShopPlayerController.h"
#include "Components/PrimitiveComponent.h"
#include "Customer/CoffeeShopCustomerCharacter.h"
#include "Customer/CoffeeShopCustomerServicePoint.h"
#include "Interaction/CoffeeShopInteractable.h"
#include "UI/CoffeeShopOrderConfirmWidget.h"
#include "Blueprint/UserWidget.h"

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

void ACoffeeShopPlayerController::OpenOrderConfirm(ACoffeeShopCustomerServicePoint* ServicePoint, AActor* CameraViewTarget)
{
	if (!IsValid(ServicePoint))
	{
		return;
	}

	// Zaten bir onay ekranı açıksa tekrar açma.
	if (IsValid(ActiveOrderConfirmWidget))
	{
		return;
	}

	// Sıradaki müşteri kasaya ulaşmadıysa ekran açılmaz (mevcut mesafe davranışıyla tutarlı).
	if (!ServicePoint->IsNextPaymentCustomerAtCounter())
	{
		UE_LOG(LogTemp, Display, TEXT("OpenOrderConfirm: next customer has not reached the counter at %s."), *GetNameSafe(ServicePoint));
		return;
	}

	ACoffeeShopCustomerCharacter* Customer = ServicePoint->GetNextPaymentCustomer();
	if (!IsValid(Customer))
	{
		return;
	}

	if (!OrderConfirmWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenOrderConfirm: OrderConfirmWidgetClass is not set on %s."), *GetNameSafe(this));
		return;
	}

	ActiveOrderConfirmWidget = CreateWidget<UCoffeeShopOrderConfirmWidget>(this, OrderConfirmWidgetClass);
	if (!ActiveOrderConfirmWidget)
	{
		return;
	}

	ActiveOrderConfirmWidget->AddToViewport();
	ActiveOrderConfirmWidget->Setup(this, ServicePoint, Customer);

	// Kamerayı POS'a yumuşak geçişle götür (sinematik "ekrana yaklaşma" hissi).
	// Mevcut hedefi sakla ki kapanınca geri dönelim.
	if (IsValid(CameraViewTarget))
	{
		PreviousViewTarget = GetViewTarget();
		SetViewTargetWithBlend(CameraViewTarget, OrderCameraBlendTime, VTBlend_Cubic);
	}

	// Mouse imleçli, sadece-UI moduna geç: oyun girişi (hareket/bakış) kesilir.
	bShowMouseCursor = true;
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(ActiveOrderConfirmWidget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

void ACoffeeShopPlayerController::CloseOrderConfirm()
{
	if (IsValid(ActiveOrderConfirmWidget))
	{
		// Widget kendini RemoveFromParent ile zaten kaldırmış olabilir; referansı temizle.
		ActiveOrderConfirmWidget = nullptr;
	}

	// Kamerayı açılıştan önceki hedefe (oyuncuya) yumuşak geçişle geri döndür.
	if (IsValid(PreviousViewTarget))
	{
		SetViewTargetWithBlend(PreviousViewTarget, OrderCameraBlendTime, VTBlend_Cubic);
		PreviousViewTarget = nullptr;
	}

	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}
