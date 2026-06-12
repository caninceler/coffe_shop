// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/CoffeeShopPlayerController.h"
#include "Components/PrimitiveComponent.h"
#include "InputCoreTypes.h"
#include "Customer/CoffeeShopCustomerCharacter.h"
#include "Customer/CoffeeShopCustomerServicePoint.h"
#include "Customer/CoffeeShopRecipeTypes.h"
#include "Interaction/CoffeeShopInteractable.h"
#include "UI/CoffeeShopOrderConfirmWidget.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "Engine/DataTable.h"
#include "EngineUtils.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"

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

// ---------------- Kahve hazırlama (tarif tabanlı) ----------------

FName ACoffeeShopPlayerController::FindNextOrderDrinkId() const
{
	// Sahnedeki ilk service point'in pickup kuyruğundaki sıradaki müşterinin içeceğini al.
	for (TActorIterator<ACoffeeShopCustomerServicePoint> It(GetWorld()); It; ++It)
	{
		ACoffeeShopCustomerServicePoint* SP = *It;
		if (!IsValid(SP))
		{
			continue;
		}
		if (ACoffeeShopCustomerCharacter* Customer = SP->GetNextDrinkCustomer())
		{
			return Customer->OrderRequest.DrinkId;
		}
	}
	return NAME_None;
}

bool ACoffeeShopPlayerController::TryPrepStep(ECoffeeStep Step)
{
	// Aktif tarif yoksa: yalnızca ilk adım (GrabCup) yeni tarif başlatabilir.
	if (ActiveDrinkId.IsNone())
	{
		if (Step != ECoffeeStep::GrabCup)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(20, 2.0f, FColor::Orange, TEXT("Önce bardak al!"));
			}
			return false;
		}

		// Sıradaki müşterinin içeceği için tarifi yükle.
		const FName DrinkId = FindNextOrderDrinkId();
		if (DrinkId.IsNone() || !RecipesDataTable)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(20, 2.0f, FColor::Orange, TEXT("Sipariş yok ya da tarif tablosu boş."));
			}
			return false;
		}
		const FCoffeeRecipeRow* Recipe = RecipesDataTable->FindRow<FCoffeeRecipeRow>(DrinkId, TEXT("TryPrepStep"));
		if (!Recipe || Recipe->Steps.Num() == 0)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(20, 2.0f, FColor::Red,
					FString::Printf(TEXT("'%s' için tarif yok!"), *DrinkId.ToString()));
			}
			return false;
		}

		ActiveDrinkId = DrinkId;
		ActiveSteps = Recipe->Steps;
		CurrentStepIndex = 0;
	}

	// Sıradaki beklenen adım bu mu?
	if (!ActiveSteps.IsValidIndex(CurrentStepIndex))
	{
		return false; // zaten bitti
	}
	const ECoffeeStep Expected = ActiveSteps[CurrentStepIndex];
	if (Step != Expected)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(20, 2.0f, FColor::Orange,
				FString::Printf(TEXT("Sırada: %d (sen: %d)"), static_cast<int32>(Expected), static_cast<int32>(Step)));
		}
		return false;
	}

	// Adımı ilerlet.
	++CurrentStepIndex;

	// İlk adım GrabCup ise eldeki bardağı oluştur.
	if (Step == ECoffeeStep::GrabCup)
	{
		SpawnCup();
	}

	if (GEngine)
	{
		const bool bReady = IsDrinkReady();
		GEngine->AddOnScreenDebugMessage(20, 2.0f, bReady ? FColor::Green : FColor::Cyan,
			bReady ? TEXT("Kahve HAZIR! Pickup'a götür.")
				   : FString::Printf(TEXT("Adım %d/%d tamam"), CurrentStepIndex, ActiveSteps.Num()));
	}
	return true;
}

bool ACoffeeShopPlayerController::IsDrinkReady() const
{
	return !ActiveDrinkId.IsNone() && CurrentStepIndex >= ActiveSteps.Num() && ActiveSteps.Num() > 0;
}

void ACoffeeShopPlayerController::ResetPreparation()
{
	ActiveDrinkId = NAME_None;
	ActiveSteps.Reset();
	CurrentStepIndex = 0;
	DestroyCup();
}

void ACoffeeShopPlayerController::SpawnCup()
{
	if (IsValid(CarriedCupActor) || !CupMeshClass)
	{
		return;
	}
	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Owner = MyPawn;
	CarriedCupActor = GetWorld()->SpawnActor<AActor>(CupMeshClass, MyPawn->GetActorTransform(), Params);
	if (!IsValid(CarriedCupActor))
	{
		return;
	}
	CarriedCupActor->SetActorEnableCollision(false);

	// Bardağı kameraya (ya da pawn'a) bağla — oyuncunun "elinde" görünsün.
	USceneComponent* AttachTo = MyPawn->GetRootComponent();
	if (UCameraComponent* Cam = MyPawn->FindComponentByClass<UCameraComponent>())
	{
		AttachTo = Cam;
	}
	if (AttachTo)
	{
		CarriedCupActor->AttachToComponent(AttachTo, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		// Kameranın önünde, hafif sağ-alt (elde tutuluyor hissi).
		CarriedCupActor->SetActorRelativeLocation(FVector(40.0f, 20.0f, -20.0f));
	}
}

void ACoffeeShopPlayerController::DestroyCup()
{
	if (IsValid(CarriedCupActor))
	{
		CarriedCupActor->Destroy();
	}
	CarriedCupActor = nullptr;
}

AActor* ACoffeeShopPlayerController::PlaceCupAt(const FTransform& DropTransform)
{
	// Hazır bardak yoksa bırakacak bir şey yok.
	if (!IsValid(CarriedCupActor))
	{
		return nullptr;
	}

	AActor* DroppedCup = CarriedCupActor;

	// Eldeki bardağı koparıp teslim noktasına koy (görünür kalır).
	DroppedCup->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	DroppedCup->SetActorTransform(DropTransform);

	// Elimizdeki referansı bırak (artık bizim değil; PickupCounter yönetir).
	CarriedCupActor = nullptr;

	// Hazırlama durumunu sıfırla ama bardağı YOK ETME (sahnede duruyor).
	ActiveDrinkId = NAME_None;
	ActiveSteps.Reset();
	CurrentStepIndex = 0;

	return DroppedCup;
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
