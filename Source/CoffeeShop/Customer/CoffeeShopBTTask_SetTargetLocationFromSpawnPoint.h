// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CoffeeShopBTTask_SetTargetLocationFromSpawnPoint.generated.h"

UCLASS()
class COFFEESHOP_API UCoffeeShopBTTask_SetTargetLocationFromSpawnPoint : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UCoffeeShopBTTask_SetTargetLocationFromSpawnPoint();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
