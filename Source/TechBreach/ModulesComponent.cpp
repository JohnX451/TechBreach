// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)


#include "ModulesComponent.h"
#include "PlayerBaseCharacter.h"
#include "AbilityComponent.h"

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
		auto SubmodArray = ActiveImplants.Find(Slot)->InstalledSubmodules;

		if (SubmodArray.Num() > 0)
		{
			InactiveSubmodules.Append(SubmodArray);
			ActiveImplants.Find(Slot)->InstalledSubmodules.Empty();
		}
		
		FImplantData RemovedModule = ActiveImplants.FindAndRemoveChecked(Slot);
		UpdateAbilityComponentOnModuleRemoval(RemovedModule.Id);
		InactiveImplants.Add(RemovedModule);
		ActiveImplants.Add(Slot, Implant);
	}

	if (Implant.Id.IsEqual(FName("AL01")) || Implant.Id.IsEqual(FName("AL02")) || Implant.Id.IsEqual(FName("AL03")))
	{
		AttachSkeletalMeshToPlayer(Implant);
	}
	
	UpdateAbilityComponentOnModuleAddition(Implant.Id);
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
	ProcessWeaponSubmodule(Submodule, false, -1);

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

	InactiveSubmodules.Add(RequestedImplant->InstalledSubmodules[Index]);

	ProcessWeaponSubmodule(RequestedImplant->InstalledSubmodules[Index], true, Index);
	RequestedImplant->InstalledSubmodules.RemoveAt(Index);
	RequestedImplant->InstalledSubmodules.Add(NewSubmodule);
	ProcessWeaponSubmodule(NewSubmodule, false, -1);

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

void UModulesComponent::AttachSkeletalMeshToPlayer(FImplantData Implant)
{
	// ToDo: append model of the servo armature if it was installed
	if (!Implant.ImplantMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("Module component: no skeletal mesh for the implant specified!"))
	} else
	{
		ACharacter* Character = Cast<ACharacter>(GetOwner());
		FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, true);
		USkeletalMeshComponent* ImplantMeshComponent = NewObject<USkeletalMeshComponent>(Character, FName("ServoArmature"));
		ImplantMeshComponent->SetupAttachment(Character->GetMesh(), Implant.ImplantSocket);
		ImplantMeshComponent->RegisterComponent();
		ImplantMeshComponent->SetSkeletalMesh(Implant.ImplantMesh);
		ImplantMeshComponent->SetRelativeLocation(Implant.ImplantRelativeLocation, false);
		ImplantMeshComponent->SetRelativeRotation(Implant.ImplantRelativeRotation);
	}

	UBaseWeaponComponent* WeaponComponent = GetOwner()->FindComponentByClass<UBaseWeaponComponent>();
	if (!WeaponComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Module component: no player weapon component found!"))
	}

	WeaponComponent->ActivateModule(Implant.MaxSubmodules);
}

void UModulesComponent::ProcessWeaponSubmodule(FSubmoduleData Submodule, bool bRemove, uint8 WeaponIndex)
{
	UBaseWeaponComponent* WeaponComponent = GetOwner()->FindComponentByClass<UBaseWeaponComponent>();
	if (!WeaponComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Module component: no player weapon component found!"))
	}

	if (bRemove)
	{
		WeaponComponent->UninstallWeapon(WeaponIndex);
	} else
	{
		WeaponComponent->InstallWeapon(Submodule.WeaponData);
	}
}

// Helper Method to find 
bool UModulesComponent::IsModuleInstalledById(FName Id)
{
	for(auto& Slot : ActiveImplants)
	{
		if(Slot.Value.Id.IsEqual(Id))
			return true;
	}
	return false;
}

//Ability Component Helper Methods
void UModulesComponent::UpdateAbilityComponentOnModuleRemoval(FName ImplantId)
{
	if(ImplantId.IsEqual(FName("H02")) || ImplantId.IsEqual(FName("H03")) || ImplantId.IsEqual(FName("AR01")))
	{
		APlayerBaseCharacter* Character = Cast<APlayerBaseCharacter>(GetOwner());
		UAbilityComponent* PlayerAbility = Character->GetAbilityComponent();

		if(ImplantId.IsEqual(FName("H02")))
		{
			PlayerAbility->ChangeAccessCodeAbilityState(false);
			UE_LOG(LogTemp, Log, TEXT("Ability Component: Deactivated AccessCode Ability"))
		}

		if(ImplantId.IsEqual(FName("AR01")))
		{
			PlayerAbility->ChangeHackingAbilityState(false);
			PlayerAbility->HackingLevel = EHackingLevel::None;
			UE_LOG(LogTemp, Log, TEXT("Ability Component: Deactivated Hacking Ability"))
		}

		if(ImplantId.IsEqual(FName("H03")))
		{
			if(IsModuleInstalledById(FName("AR01")))
			{
				PlayerAbility->HackingLevel = EHackingLevel::Basic;
				UE_LOG(LogTemp, Log, TEXT("Ability Component: Hacking Level set to Basic"))
			}
		}
	}
}

void UModulesComponent::UpdateAbilityComponentOnModuleAddition(FName ImplantId)
{
	if(ImplantId.IsEqual(FName("H02")) || ImplantId.IsEqual(FName("H03")) || ImplantId.IsEqual(FName("AR01")))
	{
		APlayerBaseCharacter* Character = Cast<APlayerBaseCharacter>(GetOwner());
		UAbilityComponent* PlayerAbility = Character->GetAbilityComponent();

		if(ImplantId.IsEqual(FName("H02")))
		{
			PlayerAbility->ChangeAccessCodeAbilityState(true);
			UE_LOG(LogTemp, Log, TEXT("Ability Component: Activated AccessCode Ability"))
		}
		if(ImplantId.IsEqual(FName("AR01")))
		{
			PlayerAbility->ChangeHackingAbilityState(true);
			UE_LOG(LogTemp, Log, TEXT("Ability Component: Activated Hacking Ability"))
			if(IsModuleInstalledById(FName("H03")))
			{
				PlayerAbility->HackingLevel = EHackingLevel::Advanced;
				UE_LOG(LogTemp, Log, TEXT("Ability Component: H03 found. Hacking Level set to Advanced"))
			}
			else
			{
				PlayerAbility->HackingLevel = EHackingLevel::Basic;
				UE_LOG(LogTemp, Log, TEXT("Ability Component: Hacking Level set to Basic"))
			}
		}
		if(ImplantId.IsEqual(FName("H03")))
		{
			if(PlayerAbility->IsHackingAbilityActive)
			{
				PlayerAbility->HackingLevel = EHackingLevel::Advanced;
				UE_LOG(LogTemp, Log, TEXT("Updated Hacking Ability: Level Advanced"))
			}
		}
	}
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
