// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CoffeeShopCustomerTypes.h"
#include "GameFramework/Character.h"
#include "CoffeeShopCustomerCharacter.generated.h"

class ACoffeeShopCustomerSeat;
class ACoffeeShopCustomerServicePoint;
class UBehaviorTree;

UCLASS(Blueprintable, BlueprintType)
class COFFEESHOP_API ACoffeeShopCustomerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACoffeeShopCustomerCharacter();

	UFUNCTION(BlueprintCallable, Category = "Customer")
	void SetVisitState(ECoffeeShopCustomerVisitState NewState);

	UFUNCTION(BlueprintPure, Category = "Customer")
	ECoffeeShopCustomerVisitState GetVisitState() const;

	UFUNCTION(BlueprintCallable, Category = "Customer")
	void SetServicePoint(ACoffeeShopCustomerServicePoint* NewServicePoint);

	UFUNCTION(BlueprintPure, Category = "Customer")
	ACoffeeShopCustomerServicePoint* GetServicePoint() const;

	UFUNCTION(BlueprintCallable, Category = "Customer")
	bool ClaimSeat(ACoffeeShopCustomerSeat* Seat);

	UFUNCTION(BlueprintCallable, Category = "Customer")
	void ReleaseSeat();

	UFUNCTION(BlueprintPure, Category = "Customer")
	ACoffeeShopCustomerSeat* GetAssignedSeat() const;

	UFUNCTION(BlueprintCallable, Category = "Customer")
	void SubmitOrder();

	UFUNCTION(BlueprintCallable, Category = "Customer")
	void AcceptPayment();

	UFUNCTION(BlueprintPure, Category = "Customer")
	bool IsPaymentAccepted() const;

	UFUNCTION(BlueprintCallable, Category = "Customer")
	void MarkOrderServed();

	UFUNCTION(BlueprintPure, Category = "Customer")
	bool IsOrderServed() const;

	UFUNCTION(BlueprintCallable, Category = "Customer")
	void StartAngryExit();

	// Müşteri oturmak mı istiyor (true), yoksa take away mı (false)? Spawn'da rastgele
	// belirlenir; oturmak isteyen boş masa bulamazsa yine take away'e döner.
	UFUNCTION(BlueprintCallable, Category = "Customer")
	void SetWantsToSitDown(bool bValue);

	UFUNCTION(BlueprintPure, Category = "Customer")
	bool WantsToSitDown() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer")
	FCoffeeShopCustomerOrderRequest OrderRequest;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Customer")
	ECoffeeShopCustomerVisitState VisitState = ECoffeeShopCustomerVisitState::None;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Customer")
	TObjectPtr<ACoffeeShopCustomerServicePoint> ServicePoint;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Customer")
	TObjectPtr<ACoffeeShopCustomerSeat> AssignedSeat;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Customer")
	bool bOrderServed = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Customer")
	bool bPaymentAccepted = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Customer")
	bool bWantsToSitDown = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer", meta = (ClampMin = "0.1", UIMin = "0.1"))
	float AngryDurationSeconds = 2.0f;

	FTimerHandle AngryExitTimerHandle;

	void FinishAngryExit();

	UFUNCTION(BlueprintImplementableEvent, Category = "Customer")
	void OnVisitStateChanged(ECoffeeShopCustomerVisitState OldState, ECoffeeShopCustomerVisitState NewState);

	UFUNCTION(BlueprintImplementableEvent, Category = "Customer")
	void OnOrderSubmitted(const FCoffeeShopCustomerOrderRequest& SubmittedOrder);
};
