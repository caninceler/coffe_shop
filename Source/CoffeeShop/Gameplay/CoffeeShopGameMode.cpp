// Copyright Epic Games, Inc. All Rights Reserved.

#include "Gameplay/CoffeeShopGameMode.h"
#include "Player/CoffeeShopHUD.h"
#include "Player/CoffeeShopPlayerCharacter.h"
#include "Player/CoffeeShopPlayerController.h"
#include "UObject/ConstructorHelpers.h"

ACoffeeShopGameMode::ACoffeeShopGameMode()
{
	DefaultPawnClass = ACoffeeShopPlayerCharacter::StaticClass();
	PlayerControllerClass = ACoffeeShopPlayerController::StaticClass();
	HUDClass = ACoffeeShopHUD::StaticClass();

	// BP_PlayerController, sipariş onay widget'ını (OrderConfirmWidgetClass) tutar.
	// World Settings / GlobalDefaultGameMode override'larına güvenmeden, C++ GameMode
	// her zaman bu BP'yi kullansın ki onay ekranı çalışsın. BP bulunamazsa C++ default
	// PlayerController'a düşer (yukarıdaki atama).
	static ConstructorHelpers::FClassFinder<APlayerController> BPPlayerControllerClass(
		TEXT("/Game/CoffeeShop/Blueprints/BP_PlayerController"));
	if (BPPlayerControllerClass.Succeeded())
	{
		PlayerControllerClass = BPPlayerControllerClass.Class;
	}
}
