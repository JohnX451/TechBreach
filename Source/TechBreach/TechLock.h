// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilityComponent.h"
#include "Interactable.h"
#include "TechLock.generated.h"

UCLASS()
class TECHBREACH_API ATechLock : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATechLock();

	// The door/locked object which can be unlocked using this lock
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock")
	TArray<AActor*> LockedActors;

	// The key which opens the lock (0 means no key is necessary)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock")
	uint8 UnlockKey;

	// Whether the lock is locked or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock")
	bool bIsLocked;

	// Whether the lock is hackable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock")
	bool bIsHackable;

	// What hacking level is required
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock")
	EHackingLevel RequiredHackingLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock")
	USoundBase* UnlockFailed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock")
	USoundBase* UnlockSuccess;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void Use_Implementation() override;
	
	void SetInteractionPopup_Implementation(bool bShow) override;

	// Check if the player has the correct code to unlock and unlock
	UFUNCTION(BlueprintCallable, Category = "Lock")
	bool TryUnlocking();

	// Check if the player has the correct hacking level to unlock and unlock
	UFUNCTION(BlueprintCallable, Category = "Lock")
	bool TryHackingUnlock();

	// Blueprint function for unlocking (callable from Blueprint)
	UFUNCTION(BlueprintCallable, Category = "Lock")
	void UnlockConsoleAction();

	// Blueprint event for unlocking (contains implementation in Blueprint, changing materials, etc.)
	UFUNCTION(BlueprintImplementableEvent, Category = "Lock")
	void UnlockConsole();

	UFUNCTION(BlueprintImplementableEvent)
	void PlaySound(USoundBase* Sound);
};
