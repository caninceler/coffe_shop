// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/CoffeeShopHUD.h"
#include "CanvasItem.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Interaction/CoffeeShopInteractable.h"
#include "Player/CoffeeShopPlayerController.h"

void ACoffeeShopHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas)
	{
		return;
	}

	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
	const FVector2D Size(4.0f, 4.0f);
	FCanvasTileItem Dot(Center - Size * 0.5f, Size, FLinearColor::White);
	Dot.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Dot);

	const ACoffeeShopPlayerController* CoffeeShopController = Cast<ACoffeeShopPlayerController>(GetOwningPlayerController());
	AActor* FocusedActor = CoffeeShopController ? CoffeeShopController->GetFocusedInteractable() : nullptr;
	if (FocusedActor)
	{
		FText Prompt = FocusedActor->GetClass()->ImplementsInterface(UCoffeeShopInteractable::StaticClass())
			? ICoffeeShopInteractable::Execute_GetInteractionPrompt(FocusedActor)
			: FText::GetEmpty();

		if (Prompt.IsEmpty() || Prompt.ToString().Equals(TEXT("Interact"), ESearchCase::IgnoreCase))
		{
			const FString ActorName = FocusedActor->GetName();
			if (ActorName.Contains(TEXT("Mug"), ESearchCase::IgnoreCase) || ActorName.Contains(TEXT("Pickup"), ESearchCase::IgnoreCase))
			{
				Prompt = FText::FromString(TEXT("Serve Coffee"));
			}
			else if (ActorName.Contains(TEXT("POS"), ESearchCase::IgnoreCase) || ActorName.Contains(TEXT("Touchscreen"), ESearchCase::IgnoreCase))
			{
				Prompt = FText::FromString(TEXT("Take Order"));
			}
		}

		const FVector2D TextPosition(Center.X - 45.0f, Center.Y + 18.0f);
		FCanvasTextItem TextItem(TextPosition, Prompt.IsEmpty() ? FText::FromString(TEXT("Interact")) : Prompt, GEngine->GetSmallFont(), FLinearColor::White);
		TextItem.EnableShadow(FLinearColor::Black);
		Canvas->DrawItem(TextItem);
	}
}
