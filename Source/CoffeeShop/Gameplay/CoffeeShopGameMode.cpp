// Copyright Epic Games, Inc. All Rights Reserved.

#include "Gameplay/CoffeeShopGameMode.h"
#include "Player/CoffeeShopHUD.h"
#include "Player/CoffeeShopPlayerCharacter.h"
#include "Player/CoffeeShopPlayerController.h"

ACoffeeShopGameMode::ACoffeeShopGameMode()
{
	DefaultPawnClass = ACoffeeShopPlayerCharacter::StaticClass();
	PlayerControllerClass = ACoffeeShopPlayerController::StaticClass();
	HUDClass = ACoffeeShopHUD::StaticClass();
}
