// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/CoffeeShopPlayerController.h"
#include "Components/PrimitiveComponent.h"
#include "InputCoreTypes.h"
#include "Customer/CoffeeShopCustomerCharacter.h"
#include "Customer/CoffeeShopCustomerServicePoint.h"
#include "Interaction/CoffeeShopInteractable.h"
#include "UI/CoffeeShopOrderConfirmWidget.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"

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
	// TEST: F tuşu eldeki mobilya türünü döngüyle değiştirir (Parça 1 testi için).
	InputComponent->BindKey(EKeys::F, IE_Pressed, this, &ACoffeeShopPlayerController::CycleCarriedFurnitureForTest);
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

	// Ekran müşteri olmasa da açılır (yönetim paneli). Müşteri sadece kasaya ULAŞMIŞSA
	// sol panelde siparişi gösterilir; ulaşmamış/yoksa müşteri null geçilir, sol panel boş kalır.
	ACoffeeShopCustomerCharacter* Customer = nullptr;
	if (ServicePoint->IsNextPaymentCustomerAtCounter())
	{
		Customer = ServicePoint->GetNextPaymentCustomer();
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

void ACoffeeShopPlayerController::SetCarriedProductId(FName ProductId)
{
	CarriedProductId = ProductId;
	UE_LOG(LogTemp, Display, TEXT("Carried product set to '%s'."), *ProductId.ToString());

	// Ekranda elindeki ürünü göster (kalıcı satır; her değişimde güncellenir).
	if (GEngine)
	{
		FString CarriedText;
		FCoffeeShopFurnitureRow Row;
		if (ProductId.IsNone())
		{
			CarriedText = TEXT("Elin boş");
		}
		else if (GetFurnitureRow(ProductId, Row))
		{
			CarriedText = FString::Printf(TEXT("Elinde: %s  [şekil: %d]"),
				*Row.DisplayName.ToString(), static_cast<int32>(Row.Shape));
		}
		else
		{
			CarriedText = FString::Printf(TEXT("Elinde: %s (tablo satırı yok!)"), *ProductId.ToString());
		}
		// Sabit anahtar (1) → satır güncellenir, üst üste yığılmaz.
		GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Yellow, CarriedText);
	}
}

void ACoffeeShopPlayerController::ConsumeCarriedFurniture()
{
	CarriedProductId = NAME_None;
}

ECoffeeShopFurnitureShape ACoffeeShopPlayerController::GetCarriedShape() const
{
	FCoffeeShopFurnitureRow Row;
	if (GetFurnitureRow(CarriedProductId, Row))
	{
		return Row.Shape;
	}
	return ECoffeeShopFurnitureShape::None;
}

bool ACoffeeShopPlayerController::GetFurnitureRow(FName ProductId, FCoffeeShopFurnitureRow& OutRow) const
{
	if (!FurnitureDataTable || ProductId.IsNone())
	{
		return false;
	}

	const FCoffeeShopFurnitureRow* Found = FurnitureDataTable->FindRow<FCoffeeShopFurnitureRow>(ProductId, TEXT("GetFurnitureRow"));
	if (!Found)
	{
		return false;
	}

	OutRow = *Found;
	return true;
}

void ACoffeeShopPlayerController::CycleCarriedFurnitureForTest()
{
	// TEST: DataTable'daki ürünleri sırayla eline al (sonra NAME_None → boş).
	if (!FurnitureDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("CycleCarriedFurnitureForTest: FurnitureDataTable not set."));
		return;
	}

	TArray<FName> RowNames = FurnitureDataTable->GetRowNames();
	if (RowNames.Num() == 0)
	{
		return;
	}

	const int32 CurrentIndex = RowNames.IndexOfByKey(CarriedProductId);
	const int32 NextIndex = CurrentIndex + 1; // -1 (bulunamadı/None) → 0
	if (RowNames.IsValidIndex(NextIndex))
	{
		SetCarriedProductId(RowNames[NextIndex]);
	}
	else
	{
		// Son üründen sonra eli boşalt.
		SetCarriedProductId(NAME_None);
	}
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
