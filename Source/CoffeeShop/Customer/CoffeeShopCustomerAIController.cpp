// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoffeeShopCustomerAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "CoffeeShopCustomerCharacter.h"
#include "CoffeeShopCustomerSeat.h"
#include "CoffeeShopCustomerServicePoint.h"

const FName ACoffeeShopCustomerAIController::KeyVisitState(TEXT("VisitState"));
const FName ACoffeeShopCustomerAIController::KeyServicePoint(TEXT("ServicePoint"));
const FName ACoffeeShopCustomerAIController::KeySeat(TEXT("Seat"));
const FName ACoffeeShopCustomerAIController::KeyTargetActor(TEXT("TargetActor"));
const FName ACoffeeShopCustomerAIController::KeyTargetLocation(TEXT("TargetLocation"));

ACoffeeShopCustomerAIController::ACoffeeShopCustomerAIController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));

	Blackboard = BlackboardComponent;
	BrainComponent = BehaviorTreeComponent;
}

void ACoffeeShopCustomerAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ACoffeeShopCustomerCharacter* Customer = GetCustomer();
	if (!Customer || !Customer->BehaviorTreeAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("Customer AI possess failed on %s: Customer=%s BehaviorTree=%s."),
			*GetNameSafe(this),
			*GetNameSafe(Customer),
			Customer ? *GetNameSafe(Customer->BehaviorTreeAsset) : TEXT("None"));
		return;
	}

	RunBehaviorTree(Customer->BehaviorTreeAsset);
	SyncBlackboardFromCustomer();
	UE_LOG(LogTemp, Display, TEXT("Customer AI %s started behavior tree %s for %s."),
		*GetNameSafe(this),
		*GetNameSafe(Customer->BehaviorTreeAsset),
		*GetNameSafe(Customer));
}

void ACoffeeShopCustomerAIController::SyncBlackboardFromCustomer()
{
	ACoffeeShopCustomerCharacter* Customer = GetCustomer();
	UBlackboardComponent* CurrentBlackboard = GetBlackboardComponent();
	if (!Customer || !CurrentBlackboard)
	{
		return;
	}

	CurrentBlackboard->SetValueAsInt(KeyVisitState, static_cast<int32>(Customer->GetVisitState()));
	CurrentBlackboard->SetValueAsObject(KeyServicePoint, Customer->GetServicePoint());
	CurrentBlackboard->SetValueAsObject(KeySeat, Customer->GetAssignedSeat());
}

void ACoffeeShopCustomerAIController::RestartCustomerBehavior()
{
	ACoffeeShopCustomerCharacter* Customer = GetCustomer();
	if (!Customer || !Customer->BehaviorTreeAsset || !BehaviorTreeComponent)
	{
		return;
	}

	BehaviorTreeComponent->RestartTree();
	SyncBlackboardFromCustomer();
}

ACoffeeShopCustomerCharacter* ACoffeeShopCustomerAIController::GetCustomer() const
{
	return Cast<ACoffeeShopCustomerCharacter>(GetPawn());
}
