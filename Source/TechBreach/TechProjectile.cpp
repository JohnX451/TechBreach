// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)


#include "TechProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/EngineTypes.h"
#include <Kismet/GameplayStatics.h>
#include "Particles/ParticleSystem.h"
#include "Engine/World.h"
#include "HitEffect.h"

// Sets default values
ATechProjectile::ATechProjectile()
{
	Damage = 10.0f;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComponent->InitSphereRadius(5.0f);
	CollisionComponent->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComponent->OnComponentHit.AddDynamic(this, &ATechProjectile::OnHit);

	CollisionComponent->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComponent->CanCharacterStepUpOn = ECB_No;

	RootComponent = CollisionComponent;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovementComponent->UpdatedComponent = CollisionComponent;
	ProjectileMovementComponent->InitialSpeed = 3000.f;
	ProjectileMovementComponent->MaxSpeed = 3000.f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
}

void ATechProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(OtherActor && (OtherActor != this) && (OtherActor != GetOwner()))
	{
		float DamageFinal = Damage;
		if (ImpactEffectDefault)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffectDefault, Hit.Location, Hit.ImpactNormal.Rotation());
		}
		if (ImpactSoundDefault)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSoundDefault, Hit.Location, Hit.ImpactNormal.Rotation());
		}

		// Spawning effects handled in hit actor, makes design a little easier
		if (OtherActor->GetClass()->ImplementsInterface(UHitEffect::StaticClass()))
		{
			bool bWeakSpotHit = false;
			FHitEffectData HitEffectData =	Cast<IHitEffect>(OtherActor)->Execute_GetHitEffectInfo(OtherActor);
			if (HitEffectData.WeakSpots.Contains(Hit.BoneName))
			{
				bWeakSpotHit = true;
				DamageFinal = Damage * HitEffectData.DamageModifier;
			}
			Cast<IHitEffect>(OtherActor)->Execute_SpawnHitEffect(OtherActor, DamageFinal, bWeakSpotHit, Hit.ImpactPoint, Hit.ImpactNormal, Hit.BoneName);
		}
		
		UGameplayStatics::ApplyPointDamage(OtherActor, DamageFinal, NormalImpulse, Hit, GetInstigatorController(), this, DamageType);
		Destroy();
	}
}


