// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TechUtilLib.generated.h"

/**
 * 
 */
UCLASS()
class TECHBREACH_API UTechUtilLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category="Config")
	static FString GetProjectVersion();
	
};
