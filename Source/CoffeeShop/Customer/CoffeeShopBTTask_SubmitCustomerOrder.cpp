// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoffeeShopBTTask_SubmitCustomerOrder.h"
#include "AIController.h"
#include "CoffeeShopCustomerCharacter.h"
#include "CoffeeShopCustomerServicePoint.h"

UCoffeeShopBTTask_SubmitCustomerOrder::UCoffeeShopBTTask_SubmitCustomerOrder()
{
	NodeName = TEXT("Submit Customer Order");
}

EBTNodeResult::Type UCoffeeShopBTTask_SubmitCustomerOrder::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* AIController = OwnerComp.GetAIOwner();
	ACoffeeShopCustomerCharacter* Customer = AIController ? Cast<ACoffeeShopCustomerCharacter>(AIController->GetPawn()) : nullptr;
	if (!Customer)
	{
		UE_LOG(LogTemp, Warning, TEXT("SubmitCustomerOrder failed: customer pawn is null."));
		return EBTNodeResult::Failed;
	}

	const ECoffeeShopCustomerVisitState VisitState = Customer->GetVisitState();
	const bool bCanSubmitOrder =
		!Customer->IsPaymentAccepted()
		&& (VisitState == ECoffeeShopCustomerVisitState::Entering
			|| VisitState == ECoffeeShopCustomerVisitState::WalkingToCounter
			|| VisitState == ECoffeeShopCustomerVisitState::Ordering);

	if (!bCanSubmitOrder)
	{
		UE_LOG(LogTemp, Display, TEXT("SubmitCustomerOrder skipped for customer %s in state %d."),
			*GetNameSafe(Customer),
			static_cast<int32>(VisitState));
		return EBTNodeResult::Failed;
	}

	Customer->SubmitOrder();
	if (ACoffeeShopCustomerServicePoint* ServicePoint = Customer->GetServicePoint())
	{
		ServicePoint->RegisterWaitingForPayment(Customer);
	}
	UE_LOG(LogTemp, Display, TEXT("Customer %s submitted order %s."), *GetNameSafe(Customer), *Customer->OrderRequest.DrinkId.ToString());

	return EBTNodeResult::Succeeded;
}
