// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CoffeeShopPlayerController.generated.h"

class ACoffeeShopCustomerServicePoint;
class UCoffeeShopOrderConfirmWidget;

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
};
