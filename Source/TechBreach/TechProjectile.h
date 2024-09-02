// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TechProjectile.generated.h"

UCLASS(Blueprintable)
class TECHBREACH_API ATechProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATechProjectile();

public:
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	TSubclassOf<class UDamageType> DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float Damage;

	/** Default impact effect */
	UPROPERTY(EditAnywhere, Category = Projectile)
	class UParticleSystem* ImpactEffectDefault;

	/** Default impact sound */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	class USoundBase* ImpactSoundDefault;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	bool bIsPassingThroughWalls = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	bool bGrowOverTime = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float FinalScale = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float MaxTravelDistance = 400.f;
	
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
};
