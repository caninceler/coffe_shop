// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoffeeShopBTTask_SetTargetLocationFromSpawnPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CoffeeShopCustomerAIController.h"
#include "CoffeeShopCustomerCharacter.h"
#include "CoffeeShopCustomerSpawnPoint.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"

UCoffeeShopBTTask_SetTargetLocationFromSpawnPoint::UCoffeeShopBTTask_SetTargetLocationFromSpawnPoint()
{
	NodeName = TEXT("Set Target Location From Spawn Point");
}

EBTNodeResult::Type UCoffeeShopBTTask_SetTargetLocationFromSpawnPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	const ACoffeeShopCustomerAIController* AIController = Cast<ACoffeeShopCustomerAIController>(OwnerComp.GetAIOwner());
	ACoffeeShopCustomerCharacter* Customer = AIController ? AIController->GetCustomer() : nullptr;
	if (!BlackboardComponent || !Customer)
	{
		return EBTNodeResult::Failed;
	}

	ACoffeeShopCustomerSpawnPoint* BestSpawnPoint = nullptr;
	float BestDistanceSquared = TNumericLimits<float>::Max();
	for (TActorIterator<ACoffeeShopCustomerSpawnPoint> It(OwnerComp.GetWorld()); It; ++It)
	{
		ACoffeeShopCustomerSpawnPoint* SpawnPoint = *It;
		const float DistanceSquared = FVector::DistSquared(Customer->GetActorLocation(), SpawnPoint->GetActorLocation());
		if (DistanceSquared < BestDistanceSquared)
		{
			BestDistanceSquared = DistanceSquared;
			BestSpawnPoint = SpawnPoint;
		}
	}

	if (!BestSpawnPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetTargetLocationFromSpawnPoint failed: no spawn point found."));
		return EBTNodeResult::Failed;
	}

	Customer->ReleaseSeat();
	Customer->SetVisitState(ECoffeeShopCustomerVisitState::Leaving);

	FVector TargetLocation = BestSpawnPoint->GetActorLocation();
	if (const UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(OwnerComp.GetWorld()))
	{
		FNavLocation ProjectedLocation;
		if (NavigationSystem->ProjectPointToNavigation(TargetLocation, ProjectedLocation, FVector(200.0f, 200.0f, 300.0f)))
		{
			TargetLocation = ProjectedLocation.Location;
		}
	}

	BlackboardComponent->SetValueAsVector(ACoffeeShopCustomerAIController::KeyTargetLocation, TargetLocation);
	UE_LOG(LogTemp, Display, TEXT("Customer BT set exit target to %s from %s."), *TargetLocation.ToString(), *GetNameSafe(BestSpawnPoint));
	return EBTNodeResult::Succeeded;
}
