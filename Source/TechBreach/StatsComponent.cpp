// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)


#include "StatsComponent.h"
#include "DamageTypeExtended.h"

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
	AttributeCoefficients.Add(EAttributeType::EnergyRegeneration, 0.1f);
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
}

void UStatsComponent::RecalculateAttributes()
{
	if (!bIsAlive) return;
	
	for (auto& Item : AttributeBaseValues)
	{
		CurrentAttributeValues.Add(Item.Key, Item.Value * AttributeCoefficients.FindRef(Item.Key));
	}
	SendUpdateEvent();
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
		CurrentEnergyUnit--;
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

	UE_LOG(LogTemp, Log, TEXT("Calculated damage: %f"), FinalDamage);
	
	return FinalDamage;
}

void UStatsComponent::SendUpdateEvent()
{
	ValuesChanged(
		EnergyCurrent / CurrentAttributeValues.FindRef(EAttributeType::EnergyMaximum),
		HealthCurrent / CurrentAttributeValues.FindRef(EAttributeType::HealthMaximum)
		);
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
}