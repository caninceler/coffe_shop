// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoffeeShopBTTask_SetCustomerSeated.h"
#include "CoffeeShopCustomerAIController.h"
#include "CoffeeShopCustomerCharacter.h"
#include "CoffeeShopCustomerSeat.h"

UCoffeeShopBTTask_SetCustomerSeated::UCoffeeShopBTTask_SetCustomerSeated()
{
	NodeName = TEXT("Set Customer Seated");
}

EBTNodeResult::Type UCoffeeShopBTTask_SetCustomerSeated::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const ACoffeeShopCustomerAIController* AIController = Cast<ACoffeeShopCustomerAIController>(OwnerComp.GetAIOwner());
	ACoffeeShopCustomerCharacter* Customer = AIController ? AIController->GetCustomer() : nullptr;
	if (!Customer)
	{
		return EBTNodeResult::Failed;
	}

	if (const ACoffeeShopCustomerSeat* Seat = Customer->GetAssignedSeat())
	{
		const FTransform SitTransform = Seat->GetSitTransform();
		Customer->SetActorLocationAndRotation(SitTransform.GetLocation(), SitTransform.GetRotation());
	}

	Customer->SetVisitState(ECoffeeShopCustomerVisitState::Seated);
	return EBTNodeResult::Succeeded;
}
