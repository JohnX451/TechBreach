// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)


#include "TechProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/EngineTypes.h"
#include <Kismet/GameplayStatics.h>
#include "Particles/ParticleSystem.h"
#include "Engine/World.h"
#include "HitEffect.h"
#include "GameFramework/Character.h"

// Sets default values
ATechProjectile::ATechProjectile()
{
	Damage = 10.0f;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComponent->InitSphereRadius(5.0f);
	CollisionComponent->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComponent->OnComponentHit.AddDynamic(this, &ATechProjectile::OnHit);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ATechProjectile::OnOverlap);
	CollisionComponent->CanCharacterStepUpOn = ECB_No;
	CollisionComponent->SetCanEverAffectNavigation(false);
	
	if (bIsPassingThroughWalls)
	{
		// Only works if hit object has "generate overlap events" turned on
		CollisionComponent->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	}
	
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
		//GEngine->AddOnScreenDebugMessage(3, 2.f, FColor::Blue, FString::Printf(TEXT("PROJECTILE: OnHit() called")));
		float DamageFinal = Damage;

		if (Cast<ACharacter>(OtherActor))
		{
			if (ImpactEffectCharacter)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffectCharacter, Hit.Location, Hit.ImpactNormal.Rotation());
			} else if (ImpactEffectDefault)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffectDefault, Hit.Location, Hit.ImpactNormal.Rotation());
			}

			if (ImpactSoundCharacter)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSoundCharacter, Hit.Location, Hit.ImpactNormal.Rotation());
			} else if (ImpactSoundDefault)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSoundDefault, Hit.Location, Hit.ImpactNormal.Rotation());
			}
		}
		else
		{
			if (ImpactEffectDefault)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffectDefault, Hit.Location, Hit.ImpactNormal.Rotation());
			}

			if (ImpactSoundDefault)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSoundDefault, Hit.Location, Hit.ImpactNormal.Rotation());
			}
		}
		
		OnProjectileHit(Hit.Location);

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
		if (!bIsPassingThroughWalls)
		{
			Destroy();
		}
	}
}

void ATechProjectile::OnOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if(OtherActor && (OtherActor != this) && (OtherActor != GetOwner()))
	{
		//GEngine->AddOnScreenDebugMessage(3, 2.f, FColor::Blue, FString::Printf(TEXT("PROJECTILE: OnOverlap() called")));
		float DamageFinal = Damage;
		
		if (Cast<ACharacter>(OtherActor))
		{
			if (ImpactEffectCharacter)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffectCharacter, Hit.Location, Hit.ImpactNormal.Rotation());
			} else if (ImpactEffectDefault)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffectDefault, Hit.Location, Hit.ImpactNormal.Rotation());
			}

			if (ImpactSoundCharacter)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSoundCharacter, Hit.Location, Hit.ImpactNormal.Rotation());
			} else if (ImpactSoundDefault)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSoundDefault, Hit.Location, Hit.ImpactNormal.Rotation());
			}
		}
		else
		{
			if (ImpactEffectDefault)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffectDefault, Hit.Location, Hit.ImpactNormal.Rotation());
			}

			if (ImpactSoundDefault)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSoundDefault, Hit.Location, Hit.ImpactNormal.Rotation());
			}
		}

		OnProjectileHit(Hit.Location);

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
		
		UGameplayStatics::ApplyPointDamage(OtherActor, DamageFinal, FVector(0.f), Hit, GetInstigatorController(), this, DamageType);
		if (!bIsPassingThroughWalls)
		{
			Destroy();
		}
	}
}


