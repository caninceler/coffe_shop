// Copyright Epic Games, Inc. All Rights Reserved.

#include "Furniture/CoffeeShopFurnitureSlot.h"
#include "Player/CoffeeShopPlayerController.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"

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
		return;
	}

	// Ürünün BP'sini yükle ve slotun transform'una snap'le.
	UClass* FurnitureClass = Row.FurnitureClass.LoadSynchronous();
	if (!FurnitureClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Slot %s: product '%s' has no furniture class."),
			*GetNameSafe(this), *CarriedProductId.ToString());
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* Furniture = GetWorld()->SpawnActor<AActor>(FurnitureClass, GetActorTransform(), SpawnParameters);
	if (!Furniture)
	{
		return;
	}

	PlacedFurniture = Furniture;
	bOccupied = true;
	UpdatePlaceholderVisual();

	PlayerController->ConsumeCarriedFurniture();

	UE_LOG(LogTemp, Display, TEXT("Slot %s: placed %s (product '%s')."),
		*GetNameSafe(this), *GetNameSafe(Furniture), *CarriedProductId.ToString());
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
