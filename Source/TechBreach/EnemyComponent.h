// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TECHBREACH_API UEnemyComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/** Handles the owning actor taking damage */
	UFUNCTION()
	void TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	/** Calls a Blueprint event on death and specifies a death type by integer value */
	UFUNCTION(BlueprintImplementableEvent)
	void Death(uint8 DeathType);

	/** Calls a Blueprint event when using ranged attack */
	UFUNCTION(BlueprintImplementableEvent)
	void RangedAttackCalled(uint8 AttackType);

	/** Calls a Blueprint event when using melee attack */
	UFUNCTION(BlueprintImplementableEvent)
	void MeleeAttackCalled(uint8 AttackType);
	
	/** The actor's alive state */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats)
	bool bIsAlive;

	/** The actor's maximum health */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	float HealthMaximum;

	/** The actor's current health */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats)
	float HealthCurrent;

	/** The actor's kinetic damage resistance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	float DamageReductionKinetic;

	/** The actor's electric damage resistance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	float DamageReductionElectric;

	/** The actor's radiation damage resistance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	float DamageReductionRadiation;

	/** The actor's heat damage resistance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	float DamageReductionHeat;

public:
	/** Add the absolute value of the specified amount to the actor's health */
	UFUNCTION(BlueprintCallable)
	void AddHealth(float Amount);

	/** Try executing a ranged attack */
	UFUNCTION(BlueprintCallable)
	void AttackRanged(uint8 AttackType);

	/** Try executing a melee attack */
	UFUNCTION(BlueprintCallable)
	void AttackMelee(uint8 AttackType);

private:
	UPROPERTY()
	FTimerHandle TimerHandle_Regeneration;
	
	UFUNCTION()
	float ProcessDamage(float Damage, const UDamageType* DamageType) const;
		
};
