// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Furniture/CoffeeShopFurnitureTypes.h"
#include "Interaction/CoffeeShopInteractable.h"
#include "CoffeeShopFurnitureSlot.generated.h"

class USceneComponent;
class UStaticMeshComponent;

/**
 * Mobilyanın yerleştirilebileceği bir nokta. Editörde konumlandırılır ve bir ŞEKİL
 * kabul eder (AcceptedShape). Oyuncu elinde uygun şekilde bir ürünle bu slota bakıp
 * E'ye basınca, o ürünün BP'si spawn edilip slotun transform'una snap'lenir.
 * Şekil uyuşmazsa kabul edilmez (yuvarlak slot dikdörtgen masayı almaz).
 */
UCLASS(Blueprintable, BlueprintType)
class COFFEESHOP_API ACoffeeShopFurnitureSlot : public AActor, public ICoffeeShopInteractable
{
	GENERATED_BODY()

public:
	ACoffeeShopFurnitureSlot();

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation() const override;
	virtual void OnInteractionFocusChanged_Implementation(bool bFocused) override;

	UFUNCTION(BlueprintPure, Category = "Furniture Slot")
	bool IsOccupied() const { return bOccupied; }

	UFUNCTION(BlueprintPure, Category = "Furniture Slot")
	ECoffeeShopFurnitureShape GetAcceptedShape() const { return AcceptedShape; }

protected:
	virtual void BeginPlay() override;

	void UpdatePlaceholderVisual();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Furniture Slot")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Furniture Slot")
	TObjectPtr<UStaticMeshComponent> Placeholder;

	// Bu slot hangi mobilya şeklini kabul eder.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Furniture Slot")
	ECoffeeShopFurnitureShape AcceptedShape = ECoffeeShopFurnitureShape::RoundTable;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Furniture Slot")
	bool bOccupied = false;

	// Bu slota karşılık gelen, sahnede ZATEN duran mobilya aktörü. Editörde bağlanır.
	// Başta gizlenir (boş dükkan); oyuncu uygun ürünü yerleştirince görünür yapılır.
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Furniture Slot")
	TObjectPtr<AActor> TargetModel;
};
