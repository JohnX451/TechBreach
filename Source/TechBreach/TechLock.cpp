// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)


#include "TechLock.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerBaseCharacter.h"
#include "TechBreachCharacter.h"
#include "Unlockable.h"

// Sets default values
ATechLock::ATechLock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATechLock::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATechLock::Use_Implementation()
{
	IInteractable::Use_Implementation();
}

void ATechLock::SetInteractionPopup_Implementation(bool bShow)
{
	IInteractable::SetInteractionPopup_Implementation(bShow);
}

bool ATechLock::TryUnlocking()
{
	// ToDo: access player's ability component and check if array of codes contains this door's code
	UAbilityComponent* PlayerAbility =  Cast<APlayerBaseCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->GetAbilityComponent();
	if (PlayerAbility->ContainsAccessCode(UnlockKey) || UnlockKey == 0)
	{
		UnlockConsoleAction();
		return true;
	}

	PlaySound(UnlockFailed);
	return false;
}

bool ATechLock::TryHackingUnlock()
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

	PlaySound(UnlockFailed);
	return false;
}

void ATechLock::UnlockConsoleAction()
{
	for (auto LockedActor : LockedActors) {
		if (IUnlockable* ActorToUnlock = Cast<IUnlockable>(LockedActor))
		{
			ActorToUnlock->Execute_Unlock(LockedActor);
		}
	}
	
	bIsLocked = false;
	PlaySound(UnlockSuccess);
	UnlockConsole();
	if (!ConnectedLocks.IsEmpty())
	{
		for (auto* ConnectedLock : ConnectedLocks)
		{
			ConnectedLock->UnlockConsoleRemote();
		}
	}
	UE_LOG(LogTemp, Log, TEXT("UnlockConsoleAction() called"));
}

void ATechLock::UnlockConsoleRemote()
{
	bIsLocked = false;
	UnlockConsole();
}
