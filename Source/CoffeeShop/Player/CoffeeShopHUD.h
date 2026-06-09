// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CoffeeShopHUD.generated.h"

UCLASS()
class COFFEESHOP_API ACoffeeShopHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
};
