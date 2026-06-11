// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CoffeeShopCustomerAIController.generated.h"

class ACoffeeShopCustomerCharacter;
class UBehaviorTreeComponent;
class UBlackboardComponent;

UCLASS()
class COFFEESHOP_API ACoffeeShopCustomerAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACoffeeShopCustomerAIController();

	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION(BlueprintCallable, Category = "Customer AI")
	void SyncBlackboardFromCustomer();

	void RestartCustomerBehavior();

	UFUNCTION(BlueprintPure, Category = "Customer AI")
	ACoffeeShopCustomerCharacter* GetCustomer() const;

	static const FName KeyVisitState;
	static const FName KeyServicePoint;
	static const FName KeySeat;
	static const FName KeyTargetActor;
	static const FName KeyTargetLocation;
	static const FName KeyTakeAway;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Customer AI")
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Customer AI")
	TObjectPtr<UBlackboardComponent> BlackboardComponent;
};
