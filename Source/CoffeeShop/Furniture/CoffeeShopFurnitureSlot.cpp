// Copyright Epic Games, Inc. All Rights Reserved.

#include "Furniture/CoffeeShopFurnitureSlot.h"
#include "Player/CoffeeShopPlayerController.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/Engine.h"

ACoffeeShopFurnitureSlot::ACoffeeShopFurnitureSlot()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// Boş slotu gösteren placeholder mesh (editörde küp/silindir atanabilir).
	Placeholder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Placeholder"));
	Placeholder->SetupAttachment(Root);
	Placeholder->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Placeholder->SetCollisionResponseToAllChannels(ECR_Ignore);
	Placeholder->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void ACoffeeShopFurnitureSlot::BeginPlay()
{
	Super::BeginPlay();

	// Boş dükkan: başta bu slota bağlı mevcut model gizli + çarpışmasız.
	if (IsValid(TargetModel))
	{
		TargetModel->SetActorHiddenInGame(true);
		TargetModel->SetActorEnableCollision(false);
	}

	UpdatePlaceholderVisual();
}

void ACoffeeShopFurnitureSlot::UpdatePlaceholderVisual()
{
	if (Placeholder)
	{
		// Doluyken placeholder gizlenir; boşken görünür (nereye konacağını gösterir).
		Placeholder->SetVisibility(!bOccupied);
	}
}

void ACoffeeShopFurnitureSlot::Interact_Implementation(AActor* Interactor)
{
	if (bOccupied)
	{
		return;
	}

	// Etkileşen oyuncunun PlayerController'ından taşınan ürünü oku.
	ACoffeeShopPlayerController* PlayerController = nullptr;
	if (const APawn* InteractorPawn = Cast<APawn>(Interactor))
	{
		PlayerController = Cast<ACoffeeShopPlayerController>(InteractorPawn->GetController());
	}
	if (!PlayerController)
	{
		return;
	}

	const FName CarriedProductId = PlayerController->GetCarriedProductId();
	if (CarriedProductId.IsNone())
	{
		return;
	}

	// Ürün satırını al (şekil + BP).
	FCoffeeShopFurnitureRow Row;
	if (!PlayerController->GetFurnitureRow(CarriedProductId, Row))
	{
		UE_LOG(LogTemp, Warning, TEXT("Slot %s: carried product '%s' not found in data table."),
			*GetNameSafe(this), *CarriedProductId.ToString());
		return;
	}

	// Şekil uyuşmuyorsa yerleştirme yok (yuvarlak slot dikdörtgen masayı almaz).
	if (Row.Shape != AcceptedShape)
	{
		UE_LOG(LogTemp, Display, TEXT("Slot %s rejects shape %d (accepts %d)."),
			*GetNameSafe(this), static_cast<int32>(Row.Shape), static_cast<int32>(AcceptedShape));
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(2, 3.0f, FColor::Red,
				FString::Printf(TEXT("Slot bu sekli kabul etmiyor (eldeki: %d, slot: %d)"),
					static_cast<int32>(Row.Shape), static_cast<int32>(AcceptedShape)));
		}
		return;
	}

	// Yerleştir: sahnede zaten duran bağlı modeli görünür + çarpışmalı yap (spawn yok).
	if (!IsValid(TargetModel))
	{
		UE_LOG(LogTemp, Warning, TEXT("Slot %s: TargetModel not set; nothing to reveal."), *GetNameSafe(this));
		return;
	}

	TargetModel->SetActorHiddenInGame(false);
	TargetModel->SetActorEnableCollision(true);

	bOccupied = true;
	UpdatePlaceholderVisual();

	PlayerController->ConsumeCarriedFurniture();

	UE_LOG(LogTemp, Display, TEXT("Slot %s: revealed %s (product '%s')."),
		*GetNameSafe(this), *GetNameSafe(TargetModel), *CarriedProductId.ToString());
}

FText ACoffeeShopFurnitureSlot::GetInteractionPrompt_Implementation() const
{
	return bOccupied ? FText::GetEmpty() : FText::FromString(TEXT("Yerleştir"));
}

void ACoffeeShopFurnitureSlot::OnInteractionFocusChanged_Implementation(bool bFocused)
{
	// Slota bakınca placeholder'ı vurgula (custom depth ile dış çizgi).
	if (Placeholder)
	{
		Placeholder->SetRenderCustomDepth(bFocused && !bOccupied);
	}
}
