// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoffeeShopBTTask_WaitForOrderServed.h"
#include "AIController.h"
#include "CoffeeShopCustomerCharacter.h"
#include "CoffeeShopCustomerServicePoint.h"

UCoffeeShopBTTask_WaitForOrderServed::UCoffeeShopBTTask_WaitForOrderServed()
{
	NodeName = TEXT("Wait For Order Served");
	bNotifyTick = true;
}

EBTNodeResult::Type UCoffeeShopBTTask_WaitForOrderServed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* AIController = OwnerComp.GetAIOwner();
	ACoffeeShopCustomerCharacter* Customer = AIController ? Cast<ACoffeeShopCustomerCharacter>(AIController->GetPawn()) : nullptr;
	if (Customer)
	{
		if (ACoffeeShopCustomerServicePoint* ServicePoint = Customer->GetServicePoint())
		{
			Customer->SetActorRotation(ServicePoint->GetPickupTransform().GetRotation());
			ServicePoint->RegisterWaitingForDrink(Customer);
		}
	}
	return Customer ? EBTNodeResult::InProgress : EBTNodeResult::Failed;
}

void UCoffeeShopBTTask_WaitForOrderServed::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const AAIController* AIController = OwnerComp.GetAIOwner();
	const ACoffeeShopCustomerCharacter* Customer = AIController ? Cast<ACoffeeShopCustomerCharacter>(AIController->GetPawn()) : nullptr;
	if (!Customer)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (Customer->IsOrderServed())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
