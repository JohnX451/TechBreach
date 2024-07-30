// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)

#pragma once

#include "CoreMinimal.h"
#include "AbilityComponent.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Unlockable.h"
#include "TechDoor.generated.h"

UCLASS()
class TECHBREACH_API ATechDoor : public AActor, public IInteractable, public IUnlockable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATechDoor();

	// How long the opening animation of the door takes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float OpenSpeedSeconds;

	// How long the closing animation of the door takes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float CloseSpeedSeconds;
	
	// Whether the door is locked or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool bIsLocked;
	
	// How long it takes for the door to close automatically after being opened (0 means the door will not close automatically)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float TimeBeforeClosing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	USoundBase* SoundOpen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	USoundBase* SoundClose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	USoundBase* SoundLocked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	USoundBase* SoundUnlock;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void Use_Implementation() override;
	
	void SetInteractionPopup_Implementation(bool bShow) override;

	void Unlock_Implementation() override;

	// Blueprint function for opening the door (callable from Blueprint)
	UFUNCTION(BlueprintCallable, Category = "Door")
	void OpenDoorAction();

	// Blueprint event for opening the door (contains implementation in Blueprint)
	UFUNCTION(BlueprintImplementableEvent, Category = "Door")
	void OpenDoor();

	// Blueprint event for closing the door
	UFUNCTION(BlueprintImplementableEvent, Category = "Door")
	void CloseDoor();

	// Given TimeBeforeClosing > 0, this closes the door after the specified amount of time
	UFUNCTION(BlueprintCallable, Category = "Door")
	void PrimeForClosing();
	
	UFUNCTION(BlueprintImplementableEvent)
	void PlaySound(USoundBase* Sound);
	
private:
	UPROPERTY()
	FTimerHandle TimerHandle_DoorReset;

	UFUNCTION()
	void CloseDoorAction();
};
