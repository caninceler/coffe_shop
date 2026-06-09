// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/CoffeeShopPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

ACoffeeShopPlayerCharacter::ACoffeeShopPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f));
	FirstPersonCamera->bUsePawnControlRotation = true;

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->bOrientRotationToMovement = false;
		Movement->MaxWalkSpeed = 420.0f;
	}
}

void ACoffeeShopPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Display, TEXT("CoffeeShop player spawned and ready for interaction: %s"), *GetNameSafe(this));

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->bShowMouseCursor = false;
		UE_LOG(LogTemp, Display, TEXT("CoffeeShop player possessed by %s"), *GetNameSafe(PlayerController));
	}
}

void ACoffeeShopPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACoffeeShopPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACoffeeShopPlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ACoffeeShopPlayerCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ACoffeeShopPlayerCharacter::LookUp);
	UE_LOG(LogTemp, Display, TEXT("CoffeeShop player input bindings installed."));
}

void ACoffeeShopPlayerCharacter::MoveForward(float Value)
{
	if (!FMath::IsNearlyZero(Value))
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ACoffeeShopPlayerCharacter::MoveRight(float Value)
{
	if (!FMath::IsNearlyZero(Value))
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ACoffeeShopPlayerCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ACoffeeShopPlayerCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}
