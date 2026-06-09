// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CoffeeShopBTTask_WaitForOrderServed.generated.h"

UCLASS()
class COFFEESHOP_API UCoffeeShopBTTask_WaitForOrderServed : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UCoffeeShopBTTask_WaitForOrderServed();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
