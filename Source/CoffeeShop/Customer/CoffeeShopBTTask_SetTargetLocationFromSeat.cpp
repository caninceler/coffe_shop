// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoffeeShopBTTask_SetTargetLocationFromSeat.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CoffeeShopCustomerAIController.h"
#include "CoffeeShopCustomerCharacter.h"
#include "CoffeeShopCustomerSeat.h"
#include "NavigationSystem.h"

UCoffeeShopBTTask_SetTargetLocationFromSeat::UCoffeeShopBTTask_SetTargetLocationFromSeat()
{
	NodeName = TEXT("Set Target Location From Seat");
}

EBTNodeResult::Type UCoffeeShopBTTask_SetTargetLocationFromSeat::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	ACoffeeShopCustomerAIController* AIController = Cast<ACoffeeShopCustomerAIController>(OwnerComp.GetAIOwner());
	ACoffeeShopCustomerCharacter* Customer = AIController ? AIController->GetCustomer() : nullptr;
	ACoffeeShopCustomerSeat* Seat = BlackboardComponent ? Cast<ACoffeeShopCustomerSeat>(BlackboardComponent->GetValueAsObject(ACoffeeShopCustomerAIController::KeySeat)) : nullptr;

	if (!Seat && Customer)
	{
		Seat = Customer->GetAssignedSeat();
	}

	if (!BlackboardComponent || !Seat)
	{
		return EBTNodeResult::Failed;
	}

	FVector TargetLocation = Seat->GetActorLocation();
	if (const UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(OwnerComp.GetWorld()))
	{
		FNavLocation ProjectedLocation;
		if (NavigationSystem->ProjectPointToNavigation(TargetLocation, ProjectedLocation, FVector(150.0f, 150.0f, 300.0f)))
		{
			TargetLocation = ProjectedLocation.Location;
		}
	}

	BlackboardComponent->SetValueAsVector(ACoffeeShopCustomerAIController::KeyTargetLocation, TargetLocation);

	if (Customer && Customer->GetVisitState() == ECoffeeShopCustomerVisitState::Seated)
	{
		Customer->SetActorLocation(TargetLocation);
	}

	UE_LOG(LogTemp, Display, TEXT("Customer BT set seat target to %s from %s."), *TargetLocation.ToString(), *GetNameSafe(Seat));
	return EBTNodeResult::Succeeded;
}
