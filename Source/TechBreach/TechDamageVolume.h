// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)

#pragma once

#include "CoreMinimal.h"
#include "TechDamageVolume.generated.h"

/**
 * Does not specify a volume, since selecting brush shapes in Blueprint causes a crash
 */
UCLASS(Blueprintable)
class TECHBREACH_API ATechDamageVolume : public AActor
{
	GENERATED_BODY()
	
public:
	// Whether the volume currently causes damage or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	bool bDamageCausing;

	// Whether the damage will be applied immediately after entering or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	bool bEntryDamage;

	// Whether to scale the damage by distance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	bool bScaleWithDistance;

	// The minimum amount of base damage caused per second
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	float DamagePerSecMin;

	// The maximum amount of base damage caused per second
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	float DamagePerSecMax;

	// The interval in which to apply damage in seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	float DamageInterval;
	
	// The damage class used to apply the damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	TSubclassOf<class UDamageTypeExtended> DamageType;

protected:
	FTimerHandle TimerHandle_DamageTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<AActor*> ActorsToDamage;

	UPROPERTY()
	float DamageDistance;
	
public:
	ATechDamageVolume();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void CausedDamageToActor(AActor* DamagedActor);

	UFUNCTION(BlueprintCallable)
	void ActorEnteredVolume(AActor* EnteringActor);

	UFUNCTION(BlueprintCallable)
	void ActorExitedVolume(AActor* ExitingActor);

	UFUNCTION(BlueprintCallable)
	void SetMaxDamageDistance(float Distance);

private:
	UFUNCTION()
	void CauseDamage(AActor* ActorToDamage);

	UFUNCTION()
	void DamageTimer();
};
