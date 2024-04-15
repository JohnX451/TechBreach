// Copyright Epic Games, Inc. All Rights Reserved.

#include "TechBreachGameMode.h"
#include "TechBreachCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATechBreachGameMode::ATechBreachGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
