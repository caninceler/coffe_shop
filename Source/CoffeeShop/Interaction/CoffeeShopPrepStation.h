// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/CoffeeShopInteractable.h"
#include "Customer/CoffeeShopRecipeTypes.h"
#include "CoffeeShopPrepStation.generated.h"

class USceneComponent;

/**
 * Bir kahve hazırlama istasyonu (öğütücü, makine, boiler, süt, bardak...). Her istasyon
 * bir adım (ProvidedStep) sağlar. Oyuncu E ile etkileşince, PlayerController'ın aktif
 * tarifinde sıradaki adım bu istasyonunkiyse ilerler; değilse uyarı verir.
 *
 * Mevcut model BP'leri (BP_Coffee_Grinder vb.) bu sınıftan reparent edilerek kullanılır.
 */
UCLASS(Blueprintable, BlueprintType)
class COFFEESHOP_API ACoffeeShopPrepStation : public AActor, public ICoffeeShopInteractable
{
	GENERATED_BODY()

public:
	ACoffeeShopPrepStation();

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation() const override;
	virtual void OnInteractionFocusChanged_Implementation(bool bFocused) override;

	UFUNCTION(BlueprintPure, Category = "Prep Station")
	ECoffeeStep GetProvidedStep() const { return ProvidedStep; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prep Station")
	TObjectPtr<USceneComponent> Root;

	// Bu istasyonun sağladığı adım (öğütücü=Grind, makine=PullEspresso, boiler=HotWater,
	// bardak=GrabCup, süt=SteamMilk). Editörde her istasyon BP'sinde ayarlanır.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prep Station")
	ECoffeeStep ProvidedStep = ECoffeeStep::Grind;

	// E etkileşim metni (ör. "Öğüt", "Espresso", "Sıcak Su", "Bardak Al"). Editörde set edilir.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prep Station")
	FText InteractionPrompt;
};
