// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CoffeeShopBTTask_FindAndClaimAvailableSeat.generated.h"

UCLASS()
class COFFEESHOP_API UCoffeeShopBTTask_FindAndClaimAvailableSeat : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UCoffeeShopBTTask_FindAndClaimAvailableSeat();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
