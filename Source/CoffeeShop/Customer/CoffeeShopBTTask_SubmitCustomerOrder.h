// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CoffeeShopBTTask_SubmitCustomerOrder.generated.h"

UCLASS()
class COFFEESHOP_API UCoffeeShopBTTask_SubmitCustomerOrder : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UCoffeeShopBTTask_SubmitCustomerOrder();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
