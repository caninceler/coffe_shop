// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoffeeShopBTTask_DecideSeatingOrTakeAway.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CoffeeShopCustomerAIController.h"
#include "CoffeeShopCustomerCharacter.h"
#include "CoffeeShopCustomerSeat.h"
#include "EngineUtils.h"

UCoffeeShopBTTask_DecideSeatingOrTakeAway::UCoffeeShopBTTask_DecideSeatingOrTakeAway()
{
	NodeName = TEXT("Decide Seating Or Take Away");
}

EBTNodeResult::Type UCoffeeShopBTTask_DecideSeatingOrTakeAway::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ACoffeeShopCustomerAIController* AIController = Cast<ACoffeeShopCustomerAIController>(OwnerComp.GetAIOwner());
	ACoffeeShopCustomerCharacter* Customer = AIController ? AIController->GetCustomer() : nullptr;
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!Customer || !BlackboardComponent)
	{
		return EBTNodeResult::Failed;
	}

	// Oturmak isteyen müşteri için boş koltuk aramayı dene.
	if (Customer->WantsToSitDown())
	{
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
				// Koltuk bulundu → oturma dalı.
				Customer->SetVisitState(ECoffeeShopCustomerVisitState::WalkingToSeat);
				BlackboardComponent->SetValueAsObject(ACoffeeShopCustomerAIController::KeySeat, Seat);
				BlackboardComponent->SetValueAsBool(ACoffeeShopCustomerAIController::KeyTakeAway, false);
				UE_LOG(LogTemp, Display, TEXT("%s will sit down at seat %s."), *GetNameSafe(Customer), *GetNameSafe(Seat));
				return EBTNodeResult::Succeeded;
			}
		}
		// Oturmak istedi ama boş koltuk yok → take away'e düş.
		UE_LOG(LogTemp, Display, TEXT("%s wanted to sit but no seat is free → take away."), *GetNameSafe(Customer));
	}

	// Take away: oturmuyor. Mevcut "Leaving" dalını kullan (spawn'a yürü → çıkış).
	// State'i doğrudan Leaving yapıyoruz ki BT'deki mevcut VisitState==Leaving dalı
	// müşteriyi çıkışa götürsün; ayrı bir take away dalı kurmaya gerek kalmaz.
	//
	// ÖNEMLİ: Bu node, WaitingForDrink Sequence'inin içinde (Find Seat'in yerinde).
	// O sequence devam ederse koltuk node'larını (Set Target From Seat, Move, Seated)
	// çalıştırmaya kalkar. Take away'de Failed dönerek sequence'i KESERİZ; state zaten
	// Leaving olduğu için bir sonraki tick'te müşteri Leaving dalına geçer.
	Customer->SetVisitState(ECoffeeShopCustomerVisitState::Leaving);
	BlackboardComponent->SetValueAsObject(ACoffeeShopCustomerAIController::KeySeat, nullptr);
	BlackboardComponent->SetValueAsBool(ACoffeeShopCustomerAIController::KeyTakeAway, true);
	UE_LOG(LogTemp, Display, TEXT("%s is taking away (leaving via spawn exit)."), *GetNameSafe(Customer));
	return EBTNodeResult::Failed;
}
