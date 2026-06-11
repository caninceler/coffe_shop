// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CoffeeShopBTTask_DecideSeatingOrTakeAway.generated.h"

/**
 * Kahve servis edildikten sonra çalışır. Müşteri oturmak istiyor VE boş koltuk varsa
 * koltuğu claim eder (Blackboard TakeAway=false, state WalkingToSeat). Aksi halde
 * (take away ister VEYA boş koltuk yok) TakeAway=true yapar ve müşteriyi çıkışa
 * yönlendirir (state Leaving). Her durumda Succeeded döner; BT dallanması
 * Blackboard'daki TakeAway bool'una göre yapılır.
 */
UCLASS()
class COFFEESHOP_API UCoffeeShopBTTask_DecideSeatingOrTakeAway : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UCoffeeShopBTTask_DecideSeatingOrTakeAway();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
