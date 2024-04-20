// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "StatsComponent.generated.h"

UENUM(BlueprintType)
enum class EAttributeType : uint8
{
	HealthMaximum,
	HealthRegeneration,
	EnergyMaximum,
	EnergyRegeneration,
	WeightMaximum,
	WalkSpeed,
	JumpVelocity,
	InstallSpeed,
	HackSpeed,
	DamageReductionKinetic,
	DamageReductionElectric,
	DamageReductionRadiation,
	DamageReductionHeat
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TECHBREACH_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatsComponent();

	UFUNCTION(BlueprintCallable)
	void RecalculateAttributes();

protected:
	/** The actor's current health */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats)
	float HealthCurrent;

	/** The actor's current energy */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats)
	float EnergyCurrent;

	/** The actor's current weight */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats)
	float WeightCurrent;

	/** The actor's alive state */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats)
	float bIsAlive;

	/** The number of energy bars available to the actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	uint8 EnergyUnits;

	/** The energy bar currently in use; it regenerates as long as it's not depleted */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats)
	uint8 CurrentEnergyUnit;

	/** The interval between health/energy regeneration steps in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	float RegenerationRate;

	/** The base values used to calculate actual attributes */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Stats)
	TMap<EAttributeType, float> AttributeBaseValues;

	/** The coefficients used with base attributes to calculate actual attributes */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Stats)
	TMap<EAttributeType, float> AttributeCoefficients;

	/** The actor's current attributes */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats)
	TMap<EAttributeType, float> CurrentAttributeValues;

	/** Handles the owning actor taking damage */
	UFUNCTION()
	void TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	/** Removes the absolute value of the specified amount from the actor's energy */
	UFUNCTION(BlueprintCallable)
	void RemoveEnergy(float Amount);
	
	/** Add the absolute value of the specified amount to the actor's health */
	UFUNCTION(BlueprintCallable)
	void AddHealth(float Amount);

	/** Add the absolute value of the specified amount to the actor's energy */
	UFUNCTION(BlueprintCallable)
	void AddEnergy(float Amount);
	
	/** Calls a Blueprint event on death and specifies a death type by integer value */
	UFUNCTION(BlueprintImplementableEvent)
	void Death(uint8 DeathType);
	
	/** Calls a Blueprint event when energy is depleted */
	UFUNCTION(BlueprintImplementableEvent)
	void EnergyDepleted();

	/** Calls a Blueprint event when values change */
	UFUNCTION(BlueprintImplementableEvent)
	void ValuesChanged(float EnergyPercent, float HealthPercent);
	
private:
	UPROPERTY()
	FTimerHandle TimerHandle_Regeneration;

	UFUNCTION()
	void RepeatingRegeneration();
	
	UFUNCTION()
	float ProcessDamage(float Damage, const UDamageType* DamageType) const;

	UFUNCTION()
	void SendUpdateEvent();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
};
