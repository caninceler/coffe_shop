// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoffeeShopCustomerSeat.generated.h"

class USceneComponent;

UCLASS(Blueprintable, BlueprintType)
class COFFEESHOP_API ACoffeeShopCustomerSeat : public AActor
{
	GENERATED_BODY()

public:
	ACoffeeShopCustomerSeat();

	UFUNCTION(BlueprintCallable, Category = "Customer Seat")
	bool ClaimSeat(AActor* Customer);

	UFUNCTION(BlueprintCallable, Category = "Customer Seat")
	void ReleaseSeat(AActor* Customer);

	// Pasif (bActive=false) koltuk hiçbir zaman müsait değildir → kimse oturamaz,
	// müşteri take away'e döner. Aktif ve boşsa müsaittir.
	UFUNCTION(BlueprintPure, Category = "Customer Seat")
	bool IsAvailable() const;

	UFUNCTION(BlueprintPure, Category = "Customer Seat")
	FTransform GetSitTransform() const;

	// Bu koltuğu tek tek aç/kapa.
	UFUNCTION(BlueprintCallable, Category = "Customer Seat")
	void SetSeatActive(bool bNewActive);

	UFUNCTION(BlueprintPure, Category = "Customer Seat")
	bool IsSeatActive() const;

	UFUNCTION(BlueprintPure, Category = "Customer Seat")
	int32 GetGroupId() const;

	// Belirli bir gruptaki TÜM koltukları toptan aç/kapa. İleride "masa satın al"
	// bu fonksiyona bağlanacak (grubu aktive et). KAPATMA isteğinde grupta oturan
	// müşteri varsa işlem reddedilir (false döner); aksi halde uygulanır (true).
	UFUNCTION(BlueprintCallable, Category = "Customer Seat", meta = (WorldContext = "WorldContextObject"))
	static bool SetSeatGroupActive(const UObject* WorldContextObject, int32 GroupId, bool bNewActive);

	// Bu gruptaki koltuklar aktif mi? (Gruptaki herhangi bir aktif koltuk varsa true.)
	// UI'da masa butonunun "Açık/Kapalı" durumunu göstermek için.
	UFUNCTION(BlueprintPure, Category = "Customer Seat", meta = (WorldContext = "WorldContextObject"))
	static bool IsSeatGroupActive(const UObject* WorldContextObject, int32 GroupId);

	// Bu grupta oturan (dolu) koltuk var mı? Doluysa grup kapatılamaz.
	UFUNCTION(BlueprintPure, Category = "Customer Seat", meta = (WorldContext = "WorldContextObject"))
	static bool IsSeatGroupOccupied(const UObject* WorldContextObject, int32 GroupId);

	// Bu koltukta şu an biri oturuyor mu?
	UFUNCTION(BlueprintPure, Category = "Customer Seat")
	bool IsOccupied() const;

protected:
	virtual void BeginPlay() override;

	// Aktiflik durumuna göre bağlı modelleri (sandalye/masa) göster/gizle.
	void ApplyActiveVisual();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Customer Seat")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Customer Seat")
	TObjectPtr<USceneComponent> SitPoint;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Customer Seat")
	TObjectPtr<AActor> OccupyingCustomer;

	// Bu koltuğun ait olduğu masa/grup numarası. Aynı GroupId'li koltuklar birlikte
	// açılıp kapatılır (bir masa = bir grup).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Customer Seat|Group")
	int32 GroupId = 0;

	// Koltuk aktif mi? Editörde set edilir; pasifse kimse oturamaz.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Customer Seat|Group")
	bool bActive = true;

	// Bu koltuğa ait görsel modeller (sandalye, masa vb.). Editörde sürükleyip
	// bağlanır. Koltuk pasifleşince bu aktörler tamamen gizlenir (görünmez +
	// çarpışmasız), aktifleşince geri görünür.
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Customer Seat|Group")
	TArray<TObjectPtr<AActor>> GroupModels;
};
