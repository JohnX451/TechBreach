// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Unlockable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UUnlockable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TECHBREACH_API IUnlockable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Unlock();
};
