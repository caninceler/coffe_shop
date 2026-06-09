// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CoffeeShopBTTask_FinishCustomerVisit.generated.h"

UCLASS()
class COFFEESHOP_API UCoffeeShopBTTask_FinishCustomerVisit : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UCoffeeShopBTTask_FinishCustomerVisit();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
