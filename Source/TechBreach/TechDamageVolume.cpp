// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)


#include "TechDamageVolume.h"
#include "TimerManager.h"
#include "DamageTypeExtended.h"
#include "Engine/DamageEvents.h"

ATechDamageVolume::ATechDamageVolume()
{
	bDamageCausing = true;
	bEntryDamage = true;
	bScaleWithDistance = false;
	DamagePerSecMin = 1.0f;
	DamagePerSecMax = 1.0f;
	DamageInterval = 1.0f;
	DamageType = UDamageTypeExtended::StaticClass();
}

void ATechDamageVolume::BeginPlay()
{
	Super::BeginPlay();
}

void ATechDamageVolume::ActorEnteredVolume(AActor* EnteringActor)
{
	ActorsToDamage.Add(EnteringActor);

	if (bDamageCausing && bEntryDamage && EnteringActor->CanBeDamaged())
	{
		CauseDamage(EnteringActor);
	}

	if (!GetWorldTimerManager().IsTimerActive(TimerHandle_DamageTimer))
	{
		GetWorldTimerManager().SetTimer(TimerHandle_DamageTimer, this, &ATechDamageVolume::DamageTimer, DamageInterval, true);
	}
}

void ATechDamageVolume::ActorExitedVolume(AActor* ExitingActor)
{
	ActorsToDamage.Remove(ExitingActor);
}

void ATechDamageVolume::SetMaxDamageDistance(float Distance)
{
	DamageDistance = Distance;
}

void ATechDamageVolume::CauseDamage(AActor* ActorToDamage)
{
	float DamagePerSec = (DamagePerSecMax != DamagePerSecMin) ? FMath::FRandRange(DamagePerSecMin, DamagePerSecMax) : DamagePerSecMax;

	if (bScaleWithDistance && DamageDistance > 0.f)
	{
		DamagePerSec = FMath::GetMappedRangeValueClamped(
			FVector2f(DamageDistance, 0.f),
			FVector2f(0.f, DamagePerSec),
			this->GetDistanceTo(ActorToDamage));
	}
	
	if (DamagePerSec > 0.f)
	{
		ActorToDamage->TakeDamage(
			DamagePerSec*DamageInterval,
			FDamageEvent(DamageType),
			nullptr,
			this);
	}
}

void ATechDamageVolume::DamageTimer()
{
	if (bDamageCausing)
	{
		for (AActor* const A : ActorsToDamage)
		{
			if (IsValid(A) && A->CanBeDamaged())
			{
				CauseDamage(A);
			}
		}
	}

	if (ActorsToDamage.Num() == 0)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_DamageTimer);
	}
}
