// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)


#include "ModulesComponent.h"

// Sets default values for this component's properties
UModulesComponent::UModulesComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	Stats = nullptr;
}


bool UModulesComponent::AddImplantToSlot(FImplantData Implant, EImplantSlot Slot)
{
	UE_LOG(LogTemp, Log, TEXT("Module component: adding new implant..."))
	switch (Implant.ImplantType)
	{
	case EImplantType::Head:
		if (Slot != EImplantSlot::Head1 && Slot != EImplantSlot::Head2) return false;
		break;
	case EImplantType::Legs:
		if (Slot != EImplantSlot::Legs) return false;
		break;
	case EImplantType::LeftArm:
		if (Slot != EImplantSlot::LeftArm) return false;
		break;
	case EImplantType::Torso:
		if (Slot != EImplantSlot::Torso1 && Slot != EImplantSlot::Torso2) return false;
		break;
	case EImplantType::RightArm:
		if (Slot != EImplantSlot::RightArm1 && Slot != EImplantSlot::RightArm2) return false;
		break;
	}

	if (IsSlotFree(Slot))
	{
		ActiveImplants.Add(Slot, Implant);
	} else
	{
		InactiveImplants.Add(ActiveImplants.FindAndRemoveChecked(Slot));
		ActiveImplants.Add(Slot, Implant);
	}
	
	Stats->UpdateCoefficients(CalculateAttributeCoefficients());

	UE_LOG(LogTemp, Log, TEXT("Module component: added implant"))
	
	return true;
}

bool UModulesComponent::AddSubmoduleToImplant(FSubmoduleData Submodule, EImplantSlot Slot)
{
	UE_LOG(LogTemp, Log, TEXT("Module component: adding new submodule..."))

	auto RequestedImplant = ActiveImplants.Find(Slot);

	if (!RequestedImplant)
	{
		UE_LOG(LogTemp, Log, TEXT("Module component: requested module does not exist"))
		return false;
	}
	
	if (!IsImplantCompatible(Submodule, *RequestedImplant)) return false;

	if (RequestedImplant->InstalledSubmodules.Num() >= RequestedImplant->MaxSubmodules)
	{
		UE_LOG(LogTemp, Log, TEXT("Module component: requested module number of maximum submodules reached"))
		return false;
	}
	
	RequestedImplant->InstalledSubmodules.Add(Submodule);

	Stats->UpdateCoefficients(CalculateAttributeCoefficients());

	UE_LOG(LogTemp, Log, TEXT("Module component: added submodule"))
	
	return true;
}

bool UModulesComponent::ReplaceSubmoduleAtIndex(FSubmoduleData NewSubmodule, EImplantSlot Slot, int Index)
{
	auto RequestedImplant = ActiveImplants.Find(Slot);

	if (!RequestedImplant)
	{
		UE_LOG(LogTemp, Log, TEXT("Module component: requested module does not exist"))
		return false;
	}

	if (!IsImplantCompatible(NewSubmodule, *RequestedImplant)) return false;

	RequestedImplant->InstalledSubmodules.RemoveAt(Index);
	RequestedImplant->InstalledSubmodules.Add(NewSubmodule);

	Stats->UpdateCoefficients(CalculateAttributeCoefficients());

	UE_LOG(LogTemp, Log, TEXT("Module component: added submodule"))

	return true;
}

bool UModulesComponent::FindCompatibleImplantsById(TArray<FName> CompatibleIds,
                                                   TMap<EImplantSlot, FImplantData>& OutCompatibleImplants)
{
	bool bFoundMatch = false;
	TMap<EImplantSlot, FImplantData> CompatibleImplantsTemp;
	for (auto& Implant : ActiveImplants)
	{
		if (CompatibleIds.Contains(Implant.Value.Id)) {
			bFoundMatch = true;
			CompatibleImplantsTemp.Add(Implant.Key, Implant.Value);
		}
	}
	OutCompatibleImplants = CompatibleImplantsTemp;
	return bFoundMatch;
}

bool UModulesComponent::IsSlotFree(EImplantSlot Slot) const
{
	return !ActiveImplants.Contains(Slot);
}

bool UModulesComponent::IsImplantCompatible(FSubmoduleData Submodule, FImplantData RequestedImplant)
{
	bool bIsCompatible = false;
	
	if (Submodule.CompatibleImplantIds.Contains(RequestedImplant.Id))
	{
		bIsCompatible = true;
	} else
	{
		UE_LOG(LogTemp, Log, TEXT("Module component: requested module not compatible with submodule to install (Id mismatch)"))
	}

	return bIsCompatible;
}

TMap<EAttributeType, float> UModulesComponent::CalculateAttributeCoefficients() const
{
	TMap<EAttributeType, float> NewCoefficients;
	NewCoefficients.Add(EAttributeType::HealthMaximum, 1.f);
	NewCoefficients.Add(EAttributeType::HealthRegeneration, 0.f);
	NewCoefficients.Add(EAttributeType::EnergyMaximum, 1.f);
	NewCoefficients.Add(EAttributeType::EnergyRegeneration, 0.01f);
	NewCoefficients.Add(EAttributeType::WeightMaximum, 1.f);
	NewCoefficients.Add(EAttributeType::WalkSpeed, 1.f);
	NewCoefficients.Add(EAttributeType::JumpVelocity, 1.f);
	NewCoefficients.Add(EAttributeType::InstallSpeed, 1.f);
	NewCoefficients.Add(EAttributeType::HackSpeed, 1.f);
	NewCoefficients.Add(EAttributeType::DamageReductionKinetic, 0.f);
	NewCoefficients.Add(EAttributeType::DamageReductionElectric, 0.f);
	NewCoefficients.Add(EAttributeType::DamageReductionRadiation, 0.f);
	NewCoefficients.Add(EAttributeType::DamageReductionHeat, 0.f);
	
	for (auto& Implant : ActiveImplants)
	{
		for (auto& Stat : Implant.Value.AttributeCoefTerms)
		{
			NewCoefficients.Add(Stat.Key, Stat.Value + NewCoefficients.FindRef(Stat.Key));
		}
		
		for (auto& Submodule : Implant.Value.InstalledSubmodules)
		{
			for (auto& Stat : Submodule.AttributeCoefTerms)
			{
				NewCoefficients.Add(Stat.Key, Stat.Value + NewCoefficients.FindRef(Stat.Key));
			}
		}
	}
	
	return NewCoefficients;
}

// Called when the game starts
void UModulesComponent::BeginPlay()
{
	Super::BeginPlay();

	Stats = Cast<UStatsComponent>(GetOwner()->GetComponentByClass(UStatsComponent::StaticClass()));
	if (!Stats)
	{
		UE_LOG(LogTemp, Error, TEXT("Module component: cannot find Stats component!"))
	}
}
