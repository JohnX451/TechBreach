// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)


#include "TechDoor.h"

#include "AbilityComponent.h"
#include "PlayerBaseCharacter.h"
#include "TechBreachCharacter.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATechDoor::ATechDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ATechDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATechDoor::Use_Implementation()
{
	IInteractable::Use_Implementation();
}

void ATechDoor::SetInteractionPopup_Implementation(bool bShow)
{
	IInteractable::SetInteractionPopup_Implementation(bShow);
}

void ATechDoor::OpenDoorAction()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_DoorReset);
	UE_LOG(LogTemp, Log, TEXT("OpenDoorAction() C++ implementation called"))
	OpenDoor();
}

void ATechDoor::PrimeForClosing()
{
	if (TimeBeforeClosing > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle_DoorReset,
			this,
			&ATechDoor::CloseDoorAction,
			TimeBeforeClosing,
			false,
			TimeBeforeClosing
		);
	}
	UE_LOG(LogTemp, Log, TEXT("PrimeForClosing() called"))
}

bool ATechDoor::TryUnlocking()
{
	// ToDo: access player's ability component and check if array of codes contains this door's code
	UAbilityComponent* PlayerAbility =  Cast<APlayerBaseCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->GetAbilityComponent();
	if (PlayerAbility->ContainsAccessCode(UnlockKey))
	{
		bIsLocked = false;
		return true;
	}

	return false;
}

bool ATechDoor::TryHackingUnlock()
{
	if(!bIsHackable)
		return false;
	
	UAbilityComponent* PlayerAbility =  Cast<APlayerBaseCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->GetAbilityComponent();

	if(PlayerAbility->CanHack(RequiredHackingLevel))
	{
		// AK: should unlock after the progress bar finishes
		//bIsLocked = false;
		return true;
	}

	return false;
}

void ATechDoor::CloseDoorAction()
{
	CloseDoor();
	UE_LOG(LogTemp, Log, TEXT("CloseDoorAction() called"))
}

