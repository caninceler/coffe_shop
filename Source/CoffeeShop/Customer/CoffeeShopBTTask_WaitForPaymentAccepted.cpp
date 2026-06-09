// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoffeeShopBTTask_WaitForPaymentAccepted.h"
#include "AIController.h"
#include "CoffeeShopCustomerCharacter.h"
#include "CoffeeShopCustomerServicePoint.h"

struct FCoffeeShopWaitForPaymentMemory
{
	float ElapsedSeconds = 0.0f;
	float PatienceSeconds = 0.0f;
};

UCoffeeShopBTTask_WaitForPaymentAccepted::UCoffeeShopBTTask_WaitForPaymentAccepted()
{
	NodeName = TEXT("Wait For Payment Accepted");
	bNotifyTick = true;
}

EBTNodeResult::Type UCoffeeShopBTTask_WaitForPaymentAccepted::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* AIController = OwnerComp.GetAIOwner();
	const ACoffeeShopCustomerCharacter* Customer = AIController ? Cast<ACoffeeShopCustomerCharacter>(AIController->GetPawn()) : nullptr;
	FCoffeeShopWaitForPaymentMemory* Memory = reinterpret_cast<FCoffeeShopWaitForPaymentMemory*>(NodeMemory);
	if (Memory)
	{
		Memory->ElapsedSeconds = 0.0f;
		Memory->PatienceSeconds = FMath::FRandRange(FMath::Min(MinPatienceSeconds, MaxPatienceSeconds), FMath::Max(MinPatienceSeconds, MaxPatienceSeconds));
	}
	return Customer ? EBTNodeResult::InProgress : EBTNodeResult::Failed;
}

void UCoffeeShopBTTask_WaitForPaymentAccepted::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const AAIController* AIController = OwnerComp.GetAIOwner();
	ACoffeeShopCustomerCharacter* Customer = AIController ? Cast<ACoffeeShopCustomerCharacter>(AIController->GetPawn()) : nullptr;
	if (!Customer)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (Customer->IsPaymentAccepted())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	FCoffeeShopWaitForPaymentMemory* Memory = reinterpret_cast<FCoffeeShopWaitForPaymentMemory*>(NodeMemory);
	if (!Memory)
	{
		return;
	}

	if (ACoffeeShopCustomerServicePoint* ServicePoint = Customer->GetServicePoint())
	{
		if (!ServicePoint->IsFirstWaitingForPayment(Customer))
		{
			Memory->ElapsedSeconds = 0.0f;
			return;
		}
	}

	Memory->ElapsedSeconds += DeltaSeconds;
	if (Memory->ElapsedSeconds >= Memory->PatienceSeconds)
	{
		Customer->StartAngryExit();
		UE_LOG(LogTemp, Display, TEXT("%s became angry after waiting %.1f seconds without payment."), *GetNameSafe(Customer), Memory->ElapsedSeconds);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}

uint16 UCoffeeShopBTTask_WaitForPaymentAccepted::GetInstanceMemorySize() const
{
	return sizeof(FCoffeeShopWaitForPaymentMemory);
}
