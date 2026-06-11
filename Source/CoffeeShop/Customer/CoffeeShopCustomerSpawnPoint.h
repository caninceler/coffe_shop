// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoffeeShopCustomerSpawnPoint.generated.h"

class ACoffeeShopCustomerCharacter;
class ACoffeeShopCustomerServicePoint;
class USceneComponent;

UCLASS(Blueprintable, BlueprintType)
class COFFEESHOP_API ACoffeeShopCustomerSpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	ACoffeeShopCustomerSpawnPoint();

	UFUNCTION(BlueprintCallable, Category = "Customer Spawn")
	ACoffeeShopCustomerCharacter* SpawnCustomer();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Customer Spawn")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Spawn")
	TSubclassOf<ACoffeeShopCustomerCharacter> CustomerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Spawn")
	TObjectPtr<ACoffeeShopCustomerServicePoint> DefaultServicePoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Spawn|Auto Spawn")
	bool bAutoSpawn = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Spawn|Auto Spawn", meta = (ClampMin = "0.1", UIMin = "0.1"))
	float MinAutoSpawnDelay = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Spawn|Auto Spawn", meta = (ClampMin = "0.1", UIMin = "0.1"))
	float MaxAutoSpawnDelay = 15.0f;

	// Müşterinin oturmak isteme olasılığı (0..1). Geri kalanı take away ister.
	// Oturmak isteyen boş masa bulamazsa yine take away'e döner.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Spawn", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float SitDownChance = 0.75f;

private:
	void SpawnCustomerAndScheduleNext();
	void ScheduleNextAutoSpawn();

	FTimerHandle AutoSpawnTimerHandle;
};
