// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)


#include "StatsComponent.h"
#include "DamageTypeExtended.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UStatsComponent::UStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// Set up default base values
	AttributeBaseValues.Add(EAttributeType::HealthMaximum, 100.f);
	AttributeBaseValues.Add(EAttributeType::HealthRegeneration, 1.f);
	AttributeBaseValues.Add(EAttributeType::EnergyMaximum, 100.f);
	AttributeBaseValues.Add(EAttributeType::EnergyRegeneration, 1.f);
	AttributeBaseValues.Add(EAttributeType::WeightMaximum, 200.f);
	AttributeBaseValues.Add(EAttributeType::WalkSpeed, 500.f);
	AttributeBaseValues.Add(EAttributeType::JumpVelocity, 400.f);
	AttributeBaseValues.Add(EAttributeType::InstallSpeed, 1.f);
	AttributeBaseValues.Add(EAttributeType::HackSpeed, 1.f);
	AttributeBaseValues.Add(EAttributeType::DamageReductionKinetic, 1.f);
	AttributeBaseValues.Add(EAttributeType::DamageReductionElectric, 1.f);
	AttributeBaseValues.Add(EAttributeType::DamageReductionRadiation, 1.f);
	AttributeBaseValues.Add(EAttributeType::DamageReductionHeat, 1.f);

	// Set up current coefficients
	AttributeCoefficients.Add(EAttributeType::HealthMaximum, 1.f);
	AttributeCoefficients.Add(EAttributeType::HealthRegeneration, 0.f);
	AttributeCoefficients.Add(EAttributeType::EnergyMaximum, 1.f);
	AttributeCoefficients.Add(EAttributeType::EnergyRegeneration, 0.01f);
	AttributeCoefficients.Add(EAttributeType::WeightMaximum, 1.f);
	AttributeCoefficients.Add(EAttributeType::WalkSpeed, 1.f);
	AttributeCoefficients.Add(EAttributeType::JumpVelocity, 1.f);
	AttributeCoefficients.Add(EAttributeType::InstallSpeed, 1.f);
	AttributeCoefficients.Add(EAttributeType::HackSpeed, 1.f);
	AttributeCoefficients.Add(EAttributeType::DamageReductionKinetic, 0.f);
	AttributeCoefficients.Add(EAttributeType::DamageReductionElectric, 0.f);
	AttributeCoefficients.Add(EAttributeType::DamageReductionRadiation, 0.f);
	AttributeCoefficients.Add(EAttributeType::DamageReductionHeat, 0.f);

	HealthCurrent = 100.f;
	EnergyCurrent = 100.f;
	WeightCurrent = 0.f;
	bIsAlive = true;
	EnergyUnits = 3;
	CurrentEnergyUnit = EnergyUnits;
	RegenerationRate = 1.0f;
}

void UStatsComponent::UpdateCoefficients(TMap<EAttributeType, float> NewAttributeCoefficients)
{
	AttributeCoefficients = NewAttributeCoefficients;
	RecalculateAttributes();
}

void UStatsComponent::TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                 AController* InstigatedBy, AActor* DamageCauser)
{
	if (!bIsAlive) return;
	
	if (Damage <= 0.f)
	{
		return;
	}
	
	HealthCurrent = FMath::Clamp(
		HealthCurrent - ProcessDamage(Damage, DamageType),
		0.f,
		CurrentAttributeValues.FindRef(EAttributeType::HealthMaximum)
		);
	
	if (HealthCurrent == 0.f)
	{
		bIsAlive = false;
		Death(0);
	}
	SendUpdateEvent();
}

void UStatsComponent::RemoveEnergy(float Amount)
{
	if (!bIsAlive || EnergyCurrent == 0.f) return;
	EnergyCurrent = FMath::Clamp(
		EnergyCurrent - FMath::Abs(Amount),
		0.f,
		CurrentAttributeValues.FindRef(EAttributeType::EnergyMaximum)
		);

	if (EnergyCurrent < ((CurrentAttributeValues.FindRef(EAttributeType::EnergyMaximum) / EnergyUnits) * (CurrentEnergyUnit - 1)))
	{
		CurrentEnergyUnit -= 1;
	}

	if (EnergyCurrent == 0.f)
	{
		CurrentEnergyUnit = 0;
		EnergyDepleted();
	}
	SendUpdateEvent();
}

void UStatsComponent::AddHealth(float Amount)
{
	if (!bIsAlive) return;
	HealthCurrent = FMath::Clamp(
		HealthCurrent + FMath::Abs(Amount),
		HealthCurrent,
		CurrentAttributeValues.FindRef(EAttributeType::HealthMaximum)
		);
	SendUpdateEvent();
}

void UStatsComponent::AddEnergy(float Amount)
{
	if (!bIsAlive) return;
	EnergyCurrent = FMath::Clamp(
		EnergyCurrent + FMath::Abs(Amount),
		EnergyCurrent,
		CurrentAttributeValues.FindRef(EAttributeType::EnergyMaximum)
		);

	if (EnergyCurrent >= ((CurrentAttributeValues.FindRef(EAttributeType::EnergyMaximum) / EnergyUnits) * (CurrentEnergyUnit - 1)))
	{
		CurrentEnergyUnit += 1;
	}
	
	SendUpdateEvent();
}

void UStatsComponent::RepeatingRegeneration()
{
	if (!bIsAlive) return;
	if (HealthCurrent < CurrentAttributeValues.FindRef(EAttributeType::HealthMaximum) && CurrentAttributeValues.FindRef(EAttributeType::HealthRegeneration) > 0.f)
	{
		HealthCurrent = FMath::Clamp(
			HealthCurrent + CurrentAttributeValues.FindRef(EAttributeType::HealthMaximum) * CurrentAttributeValues.FindRef(EAttributeType::HealthRegeneration),
			HealthCurrent,
			CurrentAttributeValues.FindRef(EAttributeType::HealthMaximum)
			);
	}

	if (EnergyCurrent < CurrentAttributeValues.FindRef(EAttributeType::EnergyMaximum))
	{
		const float MaximumEnergyCorrected = (static_cast<float>(CurrentEnergyUnit) / static_cast<float>(EnergyUnits)) * CurrentAttributeValues.FindRef(EAttributeType::EnergyMaximum);

		EnergyCurrent = FMath::Clamp(
			EnergyCurrent + CurrentAttributeValues.FindRef(EAttributeType::EnergyMaximum) * CurrentAttributeValues.FindRef(EAttributeType::EnergyRegeneration),
			EnergyCurrent,
			MaximumEnergyCorrected
			);

		GEngine->AddOnScreenDebugMessage(3, 2.f, FColor::Blue, FString::Printf(TEXT("Current energy unit: %d"), CurrentEnergyUnit));
		GEngine->AddOnScreenDebugMessage(4, 2.f, FColor::Blue, FString::Printf(TEXT("Total energy units: %d"), EnergyUnits));
		GEngine->AddOnScreenDebugMessage(2, 2.f, FColor::Blue, FString::Printf(TEXT("Maximum energy at the current cell: %f"), MaximumEnergyCorrected));
	}

	SendUpdateEvent();
}

float UStatsComponent::ProcessDamage(float Damage, const UDamageType* DamageType) const
{
	float FinalDamage = Damage;
	
	if (const UDamageTypeExtended* DamageTypeExtended = Cast<UDamageTypeExtended>(DamageType))
	{
		const float KineticDamage = DamageTypeExtended->AsKineticDamage * (1 - CurrentAttributeValues.FindRef(EAttributeType::DamageReductionKinetic)) * Damage;
		const float ElectricDamage = DamageTypeExtended->AsElectricDamage * (1 - CurrentAttributeValues.FindRef(EAttributeType::DamageReductionElectric)) * Damage;
		const float RadiationDamage = DamageTypeExtended->AsRadiationDamage * (1 - CurrentAttributeValues.FindRef(EAttributeType::DamageReductionRadiation)) * Damage;
		const float HeatDamage = DamageTypeExtended->AsHeatDamage * (1 - CurrentAttributeValues.FindRef(EAttributeType::DamageReductionHeat)) * Damage;

		FinalDamage = KineticDamage + ElectricDamage + RadiationDamage + HeatDamage;
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Taking damage: %f"), FinalDamage));
	
	return FinalDamage;
}

void UStatsComponent::SendUpdateEvent()
{
	ValuesChanged(
		EnergyCurrent / CurrentAttributeValues.FindRef(EAttributeType::EnergyMaximum),
		HealthCurrent / CurrentAttributeValues.FindRef(EAttributeType::HealthMaximum)
		);
}

void UStatsComponent::RecalculateAttributes()
{
	if (!bIsAlive) return;
	
	for (auto& Item : AttributeBaseValues)
	{
		CurrentAttributeValues.Add(Item.Key, Item.Value * AttributeCoefficients.FindRef(Item.Key));
	}

	if (UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetOwner()->GetComponentByClass(UCharacterMovementComponent::StaticClass())))
	{
		MovementComponent->MaxWalkSpeed = CurrentAttributeValues.FindRef(EAttributeType::WalkSpeed);
		MovementComponent->MaxWalkSpeedCrouched = CurrentAttributeValues.FindRef(EAttributeType::WalkSpeed) * 0.5f;
		MovementComponent->JumpZVelocity = CurrentAttributeValues.FindRef(EAttributeType::JumpVelocity);;
	}
	
	SendUpdateEvent();
}

// Called when the game starts
void UStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	// Bind owning Actor's damage function to this component's damage function
	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UStatsComponent::TakeDamage);
	}

	// Initialize current values
	RecalculateAttributes();

	HealthCurrent = CurrentAttributeValues.FindRef(EAttributeType::HealthMaximum);
	EnergyCurrent = CurrentAttributeValues.FindRef(EAttributeType::EnergyMaximum);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Regeneration, this, &UStatsComponent::RepeatingRegeneration, RegenerationRate, true, 0.0f);
}