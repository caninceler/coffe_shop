// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoffeeShopBTTask_FindAndClaimAvailableSeat.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CoffeeShopCustomerAIController.h"
#include "CoffeeShopCustomerCharacter.h"
#include "CoffeeShopCustomerSeat.h"
#include "EngineUtils.h"

UCoffeeShopBTTask_FindAndClaimAvailableSeat::UCoffeeShopBTTask_FindAndClaimAvailableSeat()
{
	NodeName = TEXT("Find And Claim Available Seat");
}

EBTNodeResult::Type UCoffeeShopBTTask_FindAndClaimAvailableSeat::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ACoffeeShopCustomerAIController* AIController = Cast<ACoffeeShopCustomerAIController>(OwnerComp.GetAIOwner());
	ACoffeeShopCustomerCharacter* Customer = AIController ? AIController->GetCustomer() : nullptr;
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!Customer || !BlackboardComponent)
	{
		return EBTNodeResult::Failed;
	}

	TArray<ACoffeeShopCustomerSeat*> AvailableSeats;

	for (TActorIterator<ACoffeeShopCustomerSeat> It(OwnerComp.GetWorld()); It; ++It)
	{
		ACoffeeShopCustomerSeat* Seat = *It;
		if (Seat && Seat->IsAvailable())
		{
			AvailableSeats.Add(Seat);
		}
	}

	while (!AvailableSeats.IsEmpty())
	{
		const int32 SeatIndex = FMath::RandRange(0, AvailableSeats.Num() - 1);
		ACoffeeShopCustomerSeat* Seat = AvailableSeats[SeatIndex];
		AvailableSeats.RemoveAtSwap(SeatIndex);

		if (Customer->ClaimSeat(Seat))
		{
			Customer->SetVisitState(ECoffeeShopCustomerVisitState::WalkingToSeat);
			BlackboardComponent->SetValueAsObject(ACoffeeShopCustomerAIController::KeySeat, Seat);
			UE_LOG(LogTemp, Display, TEXT("%s randomly claimed seat %s."), *GetNameSafe(Customer), *GetNameSafe(Seat));
			return EBTNodeResult::Succeeded;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("%s could not claim a seat."), *GetNameSafe(Customer));
	return EBTNodeResult::Failed;
}
