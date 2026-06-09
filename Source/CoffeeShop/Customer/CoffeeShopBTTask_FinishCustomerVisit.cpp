// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoffeeShopBTTask_FinishCustomerVisit.h"
#include "CoffeeShopCustomerAIController.h"
#include "CoffeeShopCustomerCharacter.h"

UCoffeeShopBTTask_FinishCustomerVisit::UCoffeeShopBTTask_FinishCustomerVisit()
{
	NodeName = TEXT("Finish Customer Visit");
}

EBTNodeResult::Type UCoffeeShopBTTask_FinishCustomerVisit::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const ACoffeeShopCustomerAIController* AIController = Cast<ACoffeeShopCustomerAIController>(OwnerComp.GetAIOwner());
	ACoffeeShopCustomerCharacter* Customer = AIController ? AIController->GetCustomer() : nullptr;
	if (!Customer)
	{
		return EBTNodeResult::Failed;
	}

	Customer->ReleaseSeat();
	Customer->Destroy();
	return EBTNodeResult::Succeeded;
}
