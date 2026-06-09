// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoffeeShopCustomerCharacter.h"
#include "CoffeeShopCustomerSeat.h"
#include "CoffeeShopCustomerServicePoint.h"
#include "CoffeeShopCustomerAIController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

ACoffeeShopCustomerCharacter::ACoffeeShopCustomerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	AIControllerClass = ACoffeeShopCustomerAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetCharacterMovement()->SetAvoidanceEnabled(false);
}

void ACoffeeShopCustomerCharacter::SetVisitState(ECoffeeShopCustomerVisitState NewState)
{
	if (VisitState == NewState)
	{
		return;
	}

	const ECoffeeShopCustomerVisitState OldState = VisitState;
	VisitState = NewState;
	if (ACoffeeShopCustomerAIController* CustomerController = Cast<ACoffeeShopCustomerAIController>(GetController()))
	{
		CustomerController->SyncBlackboardFromCustomer();
	}
	OnVisitStateChanged(OldState, NewState);
}

void ACoffeeShopCustomerCharacter::StartAngryExit()
{
	if (VisitState == ECoffeeShopCustomerVisitState::Leaving || VisitState == ECoffeeShopCustomerVisitState::Angry)
	{
		return;
	}

	SetVisitState(ECoffeeShopCustomerVisitState::Angry);
	GetCharacterMovement()->StopMovementImmediately();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			AngryExitTimerHandle,
			this,
			&ACoffeeShopCustomerCharacter::FinishAngryExit,
			AngryDurationSeconds,
			false);
	}
}

void ACoffeeShopCustomerCharacter::FinishAngryExit()
{
	if (ServicePoint)
	{
		ServicePoint->RemoveCustomerFromQueues(this);
	}

	SetVisitState(ECoffeeShopCustomerVisitState::Leaving);
	if (ACoffeeShopCustomerAIController* CustomerController = Cast<ACoffeeShopCustomerAIController>(GetController()))
	{
		CustomerController->RestartCustomerBehavior();
	}
}

ECoffeeShopCustomerVisitState ACoffeeShopCustomerCharacter::GetVisitState() const
{
	return VisitState;
}

void ACoffeeShopCustomerCharacter::SetServicePoint(ACoffeeShopCustomerServicePoint* NewServicePoint)
{
	ServicePoint = NewServicePoint;
	if (ACoffeeShopCustomerAIController* CustomerController = Cast<ACoffeeShopCustomerAIController>(GetController()))
	{
		CustomerController->SyncBlackboardFromCustomer();
	}
}

ACoffeeShopCustomerServicePoint* ACoffeeShopCustomerCharacter::GetServicePoint() const
{
	return ServicePoint;
}

bool ACoffeeShopCustomerCharacter::ClaimSeat(ACoffeeShopCustomerSeat* Seat)
{
	if (!Seat || !Seat->ClaimSeat(this))
	{
		return false;
	}

	ReleaseSeat();
	AssignedSeat = Seat;
	if (ACoffeeShopCustomerAIController* CustomerController = Cast<ACoffeeShopCustomerAIController>(GetController()))
	{
		CustomerController->SyncBlackboardFromCustomer();
	}
	return true;
}

void ACoffeeShopCustomerCharacter::ReleaseSeat()
{
	if (AssignedSeat)
	{
		AssignedSeat->ReleaseSeat(this);
		AssignedSeat = nullptr;
		if (ACoffeeShopCustomerAIController* CustomerController = Cast<ACoffeeShopCustomerAIController>(GetController()))
		{
			CustomerController->SyncBlackboardFromCustomer();
		}
	}
}

ACoffeeShopCustomerSeat* ACoffeeShopCustomerCharacter::GetAssignedSeat() const
{
	return AssignedSeat;
}

void ACoffeeShopCustomerCharacter::SubmitOrder()
{
	bOrderServed = false;
	bPaymentAccepted = false;
	if (ServicePoint)
	{
		SetActorRotation(ServicePoint->GetPaymentTransform().GetRotation());
	}
	SetVisitState(ECoffeeShopCustomerVisitState::Ordering);
	OnOrderSubmitted(OrderRequest);
}

void ACoffeeShopCustomerCharacter::AcceptPayment()
{
	bPaymentAccepted = true;
	SetVisitState(ECoffeeShopCustomerVisitState::WaitingForDrink);
}

bool ACoffeeShopCustomerCharacter::IsPaymentAccepted() const
{
	return bPaymentAccepted;
}

void ACoffeeShopCustomerCharacter::MarkOrderServed()
{
	bOrderServed = true;
}

bool ACoffeeShopCustomerCharacter::IsOrderServed() const
{
	return bOrderServed;
}
