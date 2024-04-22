// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)


#include "PlayerBaseCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "StatsComponent.h"

// Sets default values
APlayerBaseCharacter::APlayerBaseCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
}

// Called when the game starts or when spawned
void APlayerBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (const auto* StatsComponent = Cast<UStatsComponent>(GetComponentByClass(UStatsComponent::StaticClass())))
	{
		GetCharacterMovement()->MaxWalkSpeed = StatsComponent->GetAttributeValueOfType(EAttributeType::WalkSpeed);
		GetCharacterMovement()->MaxWalkSpeedCrouched = StatsComponent->GetAttributeValueOfType(EAttributeType::WalkSpeed) * 0.5f;
		GetCharacterMovement()->JumpZVelocity = StatsComponent->GetAttributeValueOfType(EAttributeType::JumpVelocity);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void APlayerBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerBaseCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerBaseCharacter::Look);

		// Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &APlayerBaseCharacter::StartCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &APlayerBaseCharacter::StopCrouch);

		// Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerBaseCharacter::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerBaseCharacter::StopSprint);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void APlayerBaseCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APlayerBaseCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerBaseCharacter::StartCrouch()
{
	Crouch();
}

void APlayerBaseCharacter::StopCrouch()
{
	UnCrouch();
}

void APlayerBaseCharacter::StartSprint()
{
	if (const auto* StatsComponent = Cast<UStatsComponent>(GetComponentByClass(UStatsComponent::StaticClass())))
	{
		GetCharacterMovement()->MaxWalkSpeed = StatsComponent->GetAttributeValueOfType(EAttributeType::WalkSpeed) * SprintSpeedCoefficient;
	}
}

void APlayerBaseCharacter::StopSprint()
{
	if (const auto* StatsComponent = Cast<UStatsComponent>(GetComponentByClass(UStatsComponent::StaticClass())))
	{
		GetCharacterMovement()->MaxWalkSpeed = StatsComponent->GetAttributeValueOfType(EAttributeType::WalkSpeed);
	}
}
