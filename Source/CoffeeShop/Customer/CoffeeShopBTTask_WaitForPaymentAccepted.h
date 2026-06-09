// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CoffeeShopBTTask_WaitForPaymentAccepted.generated.h"

UCLASS()
class COFFEESHOP_API UCoffeeShopBTTask_WaitForPaymentAccepted : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UCoffeeShopBTTask_WaitForPaymentAccepted();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;

protected:
	UPROPERTY(EditAnywhere, Category = "Customer")
	float MinPatienceSeconds = 15.0f;

	UPROPERTY(EditAnywhere, Category = "Customer")
	float MaxPatienceSeconds = 25.0f;
};
