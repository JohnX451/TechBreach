// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)


#include "EnemyComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "DamageTypeExtended.h"
#include "BaseWeaponComponent.h"

UEnemyComponent::UEnemyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	HealthMaximum = 100.f;
	DamageReductionElectric = 0.f;
	DamageReductionHeat = 0.f;
	DamageReductionKinetic = 0.f;
	DamageReductionRadiation = 0.f;
	
	HealthCurrent = HealthMaximum;

	bIsAlive = true;
}

void UEnemyComponent::BeginPlay()
{
	Super::BeginPlay();

	// Bind owning Actor's damage function to this component's damage function
	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UEnemyComponent::TakeDamage);
	}
	
}

void UEnemyComponent::TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
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
		HealthMaximum
		);
	
	if (HealthCurrent == 0.f)
	{
		bIsAlive = false;
		Death(0);
	}
}

void UEnemyComponent::AddHealth(float Amount)
{
	if (!bIsAlive) return;
	HealthCurrent = FMath::Clamp(
		HealthCurrent + FMath::Abs(Amount),
		HealthCurrent,
		HealthMaximum
		);
}

void UEnemyComponent::AttackRanged(uint8 AttackType)
{
	if (UBaseWeaponComponent* WeaponComponent = GetOwner()->FindComponentByClass<UBaseWeaponComponent>())
	{
		WeaponComponent->RequestFire();
		RangedAttackCalled(0);
	}
}

void UEnemyComponent::AttackMelee(uint8 AttackType)
{
	MeleeAttackCalled(0);
}

float UEnemyComponent::ProcessDamage(float Damage, const UDamageType* DamageType) const
{
	float FinalDamage = Damage;
	
	if (const UDamageTypeExtended* DamageTypeExtended = Cast<UDamageTypeExtended>(DamageType))
	{
		const float KineticDamage = DamageTypeExtended->AsKineticDamage * (1 - DamageReductionKinetic) * Damage;
		const float ElectricDamage = DamageTypeExtended->AsElectricDamage * (1 - DamageReductionElectric) * Damage;
		const float RadiationDamage = DamageTypeExtended->AsRadiationDamage * (1 - DamageReductionRadiation) * Damage;
		const float HeatDamage = DamageTypeExtended->AsHeatDamage * (1 - DamageReductionHeat) * Damage;

		FinalDamage = KineticDamage + ElectricDamage + RadiationDamage + HeatDamage;
	}
	
	return FinalDamage;
}
