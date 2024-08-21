// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "DamageTypeExtended.generated.h"

/**
 * Standard damage type class, but with some additional variables for differentiating between damage sources
 */
UCLASS()
class TECHBREACH_API UDamageTypeExtended : public UDamageType
{
	GENERATED_BODY()

public:
	/** The proportion of damage that is passed as kinetic damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage Types")
	float AsKineticDamage = 1.0f;

	/** The proportion of damage that is passed as electric damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage Types")
	float AsElectricDamage = 0.0f;

	/** The proportion of damage that is passed as radiation damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage Types")
	float AsRadiationDamage = 0.0f;

	/** The proportion of damage that is passed as heat damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage Types")
	float AsHeatDamage = 0.0f;

	/** Whether the afflicted character should have a pain reaction (sound/anim) or not */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage Types")
	bool bCausePainReaction = true;
};
