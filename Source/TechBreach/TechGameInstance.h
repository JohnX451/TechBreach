// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TechGameInstance.generated.h"


USTRUCT(BlueprintType)
struct FGameplayOptions
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealthCoefPlayer = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealthCoefEnemy = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MouseSensitivity = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MouseYSign = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShowSubtitles = true;
};

UCLASS()
class TECHBREACH_API UTechGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayOptions GameplayOptions;
};
