// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitEffect.generated.h"

USTRUCT(BlueprintType)
struct FHitEffectData
{
	GENERATED_BODY()

	// Coefficient for multiplying the projectile's base damage (used for weak spot damage calculation)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageModifier = 1.f;

	// Array containing skeletal mesh bone names which correspond to weak spots
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> WeakSpots;
	
	// Hit data from the original impact
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHitResult HitResult;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHitEffect : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TECHBREACH_API IHitEffect
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FHitEffectData GetHitEffectInfo();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SpawnHitEffect(float Damage, bool bIsWeakSpot, FVector HitLocation, FVector HitNormal, FName HitBone);
};
