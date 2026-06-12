// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Furniture/CoffeeShopFurnitureTypes.h"
#include "Customer/CoffeeShopRecipeTypes.h"
#include "CoffeeShopPlayerController.generated.h"

class ACoffeeShopCustomerServicePoint;
class UCoffeeShopOrderConfirmWidget;
class UDataTable;

UCLASS()
class COFFEESHOP_API ACoffeeShopPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACoffeeShopPlayerController();

	virtual void PlayerTick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "Interaction")
	AActor* GetFocusedInteractable() const;

	// "Take Order": sıradaki müşteri kasaya ulaştıysa onay ekranını açar, mouse
	// imlecini gösterir ve oyunu UI input moduna alır. CameraViewTarget verilirse
	// kamera o aktöre yumuşak geçişle kayar (POS'a yaklaşma hissi). Ulaşmadıysa
	// hiçbir şey yapmaz.
	UFUNCTION(BlueprintCallable, Category = "Order Confirm")
	void OpenOrderConfirm(ACoffeeShopCustomerServicePoint* ServicePoint, AActor* CameraViewTarget);

	// Onay ekranı kapanırken oyunu Game input moduna döndürür, imleci gizler.
	UFUNCTION(BlueprintCallable, Category = "Order Confirm")
	void CloseOrderConfirm();

	// --- Mobilya taşıma/yerleştirme ---

	// Oyuncunun elinde taşıdığı ürünün ID'si (NAME_None = boş).
	UFUNCTION(BlueprintPure, Category = "Furniture")
	FName GetCarriedProductId() const { return CarriedProductId; }

	// Elde mobilya var mı?
	UFUNCTION(BlueprintPure, Category = "Furniture")
	bool IsCarryingFurniture() const { return !CarriedProductId.IsNone(); }

	// Taşınan ürünün şekli (slot uyumu için). Ürün yoksa None.
	UFUNCTION(BlueprintPure, Category = "Furniture")
	ECoffeeShopFurnitureShape GetCarriedShape() const;

	// Oyuncuya bir ürün ID'si ver (kutudan alınca / test tuşuyla).
	UFUNCTION(BlueprintCallable, Category = "Furniture")
	void SetCarriedProductId(FName ProductId);

	// Taşınan ürünü "tüket" (slota yerleştirilince) → elini boşaltır.
	UFUNCTION(BlueprintCallable, Category = "Furniture")
	void ConsumeCarriedFurniture();

	// Bir ürün ID'sinin DataTable satırını döndürür (slot, spawn için kullanır).
	// Bulunamazsa false.
	bool GetFurnitureRow(FName ProductId, FCoffeeShopFurnitureRow& OutRow) const;

	// --- Kahve hazırlama (tarif tabanlı) ---

	// Bir istasyon adımını dene. Tarifte sıradaki adım buysa ilerler (true); değilse uyarı (false).
	// İlk adım (GrabCup) yeni bir tarif başlatır (sıradaki müşterinin içeceği).
	UFUNCTION(BlueprintCallable, Category = "Coffee")
	bool TryPrepStep(ECoffeeStep Step);

	// Elde hazır kahve var mı? (Aktif tarifin tüm adımları bitti mi.)
	UFUNCTION(BlueprintPure, Category = "Coffee")
	bool IsDrinkReady() const;

	// Bir hazırlama süreci aktif mi (bardak alındı, henüz bitmedi/teslim edilmedi)?
	UFUNCTION(BlueprintPure, Category = "Coffee")
	bool IsPreparingDrink() const { return !ActiveDrinkId.IsNone(); }

	// Hazırlanan içeceğin ID'si (doğruluk kontrolü için; şimdilik pasif).
	UFUNCTION(BlueprintPure, Category = "Coffee")
	FName GetReadyDrinkId() const { return ActiveDrinkId; }

	// Hazırlamayı sıfırla (servis edilince ya da iptal) — eldeki bardağı yok eder.
	UFUNCTION(BlueprintCallable, Category = "Coffee")
	void ResetPreparation();

	// Hazır bardağı teslim noktasına bırak: bardak elden çıkar, verilen transform'a
	// snap'lenir ve görünür kalır; PickupCounter NPC alınca onu yok eder. Hazırlama
	// durumu sıfırlanır (ActiveDrinkId temizlenir) ama bardak aktörü sahnede DURUR.
	// Bırakılan bardak aktörünü döndürür (PickupCounter sonra Destroy eder), yoksa null.
	UFUNCTION(BlueprintCallable, Category = "Coffee")
	AActor* PlaceCupAt(const FTransform& DropTransform);

protected:
	virtual void SetupInputComponent() override;

	void Interact();
	void UpdateFocusedInteractable();
	void SetFocusedInteractable(AActor* NewFocusedActor);
	void SetActorHighlight(AActor* Actor, bool bHighlighted) const;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionDistance = 500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	uint8 FocusStencilValue = 1;

	UPROPERTY()
	TObjectPtr<AActor> FocusedInteractable;

	// BP_PlayerController'da WBP_OrderConfirm'e set edilir.
	UPROPERTY(EditDefaultsOnly, Category = "Order Confirm")
	TSubclassOf<UCoffeeShopOrderConfirmWidget> OrderConfirmWidgetClass;

	// Onay ekranı açılırken/kapanırken kamera geçiş süresi (saniye).
	UPROPERTY(EditDefaultsOnly, Category = "Order Confirm", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float OrderCameraBlendTime = 0.4f;

	// Şu an açık olan onay ekranı (varsa).
	UPROPERTY()
	TObjectPtr<UCoffeeShopOrderConfirmWidget> ActiveOrderConfirmWidget;

	// Onay ekranı açılmadan önceki kamera hedefi; kapanınca buna geri dönülür.
	UPROPERTY()
	TObjectPtr<AActor> PreviousViewTarget;

	// Oyuncunun elinde taşıdığı ürünün ID'si (DataTable RowName).
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Furniture")
	FName CarriedProductId = NAME_None;

	// Tüm mobilya ürünlerini tanımlayan DataTable (satır tipi: FCoffeeShopFurnitureRow).
	// Editörde (BP_PlayerController) atanır.
	UPROPERTY(EditDefaultsOnly, Category = "Furniture")
	TObjectPtr<UDataTable> FurnitureDataTable;

	// TEST için: bu tuş DataTable'daki ürünleri sırayla eline alır (Parça 2'de kutu gelince kalkar).
	void CycleCarriedFurnitureForTest();

	// --- Kahve hazırlama state'i ---

	// Tüm içecek tariflerini tanımlayan DataTable (satır tipi: FCoffeeRecipeRow). Editörde atanır.
	UPROPERTY(EditDefaultsOnly, Category = "Coffee")
	TObjectPtr<UDataTable> RecipesDataTable;

	// Hazır olunca oyuncunun eline takılacak bardak mesh aktörü (BP_Mug). Editörde atanır.
	UPROPERTY(EditDefaultsOnly, Category = "Coffee")
	TSubclassOf<AActor> CupMeshClass;

	// Şu an hazırlanan içeceğin ID'si (NAME_None = hazırlık yok).
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Coffee")
	FName ActiveDrinkId = NAME_None;

	// Aktif tarifte kaçıncı adımdayız (0 = henüz adım yapılmadı).
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Coffee")
	int32 CurrentStepIndex = 0;

	// Aktif tarifin adım listesi (DataTable'dan kopyalanır).
	TArray<ECoffeeStep> ActiveSteps;

	// Elde taşınan bardak aktörü (görsel).
	UPROPERTY()
	TObjectPtr<AActor> CarriedCupActor;

	// Hazırlamak için sıradaki müşterinin içeceğini bul (sahnedeki service point kuyruğundan).
	FName FindNextOrderDrinkId() const;

	// Eldeki bardağı oluştur/yok et (görsel).
	void SpawnCup();
	void DestroyCup();
};
