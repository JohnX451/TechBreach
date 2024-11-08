// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "PlayerBaseCharacter.generated.h"

class UTechGameInstance;
class USpringArmComponent;
class UCameraComponent;
class UInteractionComponent;
class UAbilityComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS(config = Game)
class TECHBREACH_API APlayerBaseCharacter : public ACharacter
{
	GENERATED_BODY()

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	/** Use Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* UseAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	UInteractionComponent* InteractionComponent;

	/** Abilities */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UAbilityComponent* AbilityComponent;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	float SprintSpeedCoefficient = 1.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	float MouseSensitivity = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	float MouseYSign = 1.f;

public:
	// Sets default values for this character's properties
	APlayerBaseCharacter();

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void StartCrouch();

	void StopCrouch();

	void StartSprint();

	void StopSprint();

	void Interact();

public:
	/** Return Ability subobject **/
	FORCEINLINE class UAbilityComponent* GetAbilityComponent() const { return AbilityComponent; }
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

private:
	UPROPERTY()
	const UTechGameInstance* GameInstance;
};