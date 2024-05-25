// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)


#include "AbilityComponent.h"

UAbilityComponent::UAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	HackingLevel = EHackingLevel::None;
}


// AccessCode Management
void UAbilityComponent::ChangeAccessCodeAbilityState(bool Active)
{
	if(Active == false)
		ClearAccessCodes(); //Should the player lose all access codes after uninstalling the module?
	IsAccessCodeAbilityActive = Active;
}

bool UAbilityComponent::AddAccessCode(uint8 AccessCode)
{
	if(IsAccessCodeAbilityActive && !ScannedAccessCode.Contains(AccessCode))
	{
		ScannedAccessCode.Add(AccessCode);
		return true;
	}
	return false;
}

bool UAbilityComponent::ContainsAccessCode(uint8 AccessCode)
{
	if(!IsAccessCodeAbilityActive)
		return false;
	
	return ScannedAccessCode.Contains(AccessCode);
}

void UAbilityComponent::ClearAccessCodes()
{
	ScannedAccessCode.Empty();
}

// Hacking Management
void UAbilityComponent::ChangeHackingAbilityState(bool Active)
{
	if(Active == false)
	{
		HackingLevel = EHackingLevel::None;
	}
	else
	{
		HackingLevel = EHackingLevel::Basic;
	}

	IsHackingAbilityActive = Active;
}

void UAbilityComponent::UpdateHackingLevel(EHackingLevel UpdatedHackingLevel)
{
	HackingLevel = UpdatedHackingLevel;
}

bool UAbilityComponent::CanHack(EHackingLevel RequiredHackingLevel)
{
	if(!IsHackingAbilityActive)
		return false;
	
	return HackingLevel >= RequiredHackingLevel;
}

void UAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
}
