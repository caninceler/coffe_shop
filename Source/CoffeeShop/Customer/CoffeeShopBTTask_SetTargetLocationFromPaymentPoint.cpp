// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoffeeShopBTTask_SetTargetLocationFromPaymentPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CoffeeShopCustomerAIController.h"
#include "CoffeeShopCustomerCharacter.h"
#include "CoffeeShopCustomerServicePoint.h"
#include "EngineUtils.h"

namespace
{
ACoffeeShopCustomerServicePoint* ResolveCustomerServicePoint(UBehaviorTreeComponent& OwnerComp, UBlackboardComponent& BlackboardComponent)
{
	ACoffeeShopCustomerServicePoint* ServicePoint = Cast<ACoffeeShopCustomerServicePoint>(
		BlackboardComponent.GetValueAsObject(ACoffeeShopCustomerAIController::KeyServicePoint));
	if (ServicePoint)
	{
		return ServicePoint;
	}

	if (const ACoffeeShopCustomerAIController* AIController = Cast<ACoffeeShopCustomerAIController>(OwnerComp.GetAIOwner()))
	{
		if (const ACoffeeShopCustomerCharacter* Customer = AIController->GetCustomer())
		{
			ServicePoint = Customer->GetServicePoint();
			if (ServicePoint)
			{
				BlackboardComponent.SetValueAsObject(ACoffeeShopCustomerAIController::KeyServicePoint, ServicePoint);
				UE_LOG(LogTemp, Warning, TEXT("Customer BT recovered ServicePoint from customer %s."), *GetNameSafe(Customer));
				return ServicePoint;
			}
		}
	}

	UWorld* World = OwnerComp.GetWorld();
	if (World)
	{
		for (TActorIterator<ACoffeeShopCustomerServicePoint> It(World); It; ++It)
		{
			ServicePoint = *It;
			BlackboardComponent.SetValueAsObject(ACoffeeShopCustomerAIController::KeyServicePoint, ServicePoint);
			UE_LOG(LogTemp, Warning, TEXT("Customer BT recovered ServicePoint from world: %s."), *GetNameSafe(ServicePoint));
			return ServicePoint;
		}
	}

	return nullptr;
}
}

UCoffeeShopBTTask_SetTargetLocationFromPaymentPoint::UCoffeeShopBTTask_SetTargetLocationFromPaymentPoint()
{
	NodeName = TEXT("Set Target Location From Payment Point");
}

EBTNodeResult::Type UCoffeeShopBTTask_SetTargetLocationFromPaymentPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetTargetLocationFromPaymentPoint failed: BlackboardComponent is null."));
		return EBTNodeResult::Failed;
	}

	ACoffeeShopCustomerServicePoint* ServicePoint = ResolveCustomerServicePoint(OwnerComp, *BlackboardComponent);
	if (!ServicePoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetTargetLocationFromPaymentPoint failed: ServicePoint is null."));
		return EBTNodeResult::Failed;
	}

	ACoffeeShopCustomerCharacter* Customer = nullptr;
	if (const ACoffeeShopCustomerAIController* AIController = Cast<ACoffeeShopCustomerAIController>(OwnerComp.GetAIOwner()))
	{
		Customer = AIController->GetCustomer();
	}

	if (Customer)
	{
		const ECoffeeShopCustomerVisitState VisitState = Customer->GetVisitState();
		const bool bCanUsePaymentPoint =
			!Customer->IsPaymentAccepted()
			&& (VisitState == ECoffeeShopCustomerVisitState::Entering
				|| VisitState == ECoffeeShopCustomerVisitState::WalkingToCounter
				|| VisitState == ECoffeeShopCustomerVisitState::Ordering);

		if (!bCanUsePaymentPoint)
		{
			UE_LOG(LogTemp, Display, TEXT("SetTargetLocationFromPaymentPoint skipped for customer %s in state %d."),
				*GetNameSafe(Customer),
				static_cast<int32>(VisitState));
			return EBTNodeResult::Failed;
		}
	}

	const FVector TargetLocation = ServicePoint->GetPaymentQueueLocationForCustomer(Customer);
	BlackboardComponent->SetValueAsVector(
		ACoffeeShopCustomerAIController::KeyTargetLocation,
		TargetLocation);

	UE_LOG(LogTemp, Display, TEXT("Customer BT set payment target to %s from %s."), *TargetLocation.ToString(), *GetNameSafe(ServicePoint));

	return EBTNodeResult::Succeeded;
}
