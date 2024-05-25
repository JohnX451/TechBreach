// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilityComponent.generated.h"

UENUM(BlueprintType)
enum class EHackingLevel : uint8
{
	None,
	Basic,
	Advanced
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TECHBREACH_API UAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAbilityComponent();
	
	// AccessCode Management
public:
	UFUNCTION(BlueprintCallable, Category = "AccessCode")
	void ChangeAccessCodeAbilityState(bool Active);

	UPROPERTY(BlueprintReadOnly, Category = "AccessCode")
	bool IsAccessCodeAbilityActive = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AccessCode")
	TArray<uint8> ScannedAccessCode;

	UFUNCTION(BlueprintCallable, Category = "AccessCode")
	bool AddAccessCode(uint8 AccessCode);
	
	UFUNCTION(BlueprintCallable, Category = "AccessCode")
	bool ContainsAccessCode(uint8 AccessCode);

	UFUNCTION(BlueprintCallable, Category = "AccessCode")
	void ClearAccessCodes();

	// Hacking Management
public:
	UFUNCTION(BlueprintCallable, Category = "Hacking")
	void ChangeHackingAbilityState(bool Active);
	
	UPROPERTY(BlueprintReadOnly, Category = "Hacking")
	bool IsHackingAbilityActive = false;

	UPROPERTY(BlueprintReadOnly, Category = "Hacking")
	EHackingLevel HackingLevel;

	UFUNCTION(BlueprintCallable, Category = "Hacking")
	void UpdateHackingLevel(EHackingLevel UpdatedHackingLevel);
	
	UFUNCTION(BlueprintCallable, Category = "Hacking")
	bool CanHack(EHackingLevel RequiredHackingLevel);
	
protected:
	virtual void BeginPlay() override;
};
