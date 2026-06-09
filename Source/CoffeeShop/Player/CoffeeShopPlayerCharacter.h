// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CoffeeShopPlayerCharacter.generated.h"

class UCameraComponent;

UCLASS(Blueprintable, BlueprintType)
class COFFEESHOP_API ACoffeeShopPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACoffeeShopPlayerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FirstPersonCamera;
};
