// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseWeaponComponent.generated.h"

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	AreaOfEffect,
	Projectile,
	InstantHit,
	Beam
};

USTRUCT(BlueprintType)
struct FDamageDataAoE
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UDamageType> DamageType;
	
	UPROPERTY(EditAnywhere, Category = "Projectile")
	class UParticleSystem* ImpactEffectDefault;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;
	
	/** For AoE weapons: the radius in cm in which the weapon applies full damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float RadiusInnerCm;
	
	/** For AoE weapons: the maximum radius of the weapon in cm */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float RadiusCm;

	/** For AoE weapons: actors caught within this angle take damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float AngleDegrees;
};

USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDamageDataAoE AoEData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMesh* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* FireEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFireMode FireMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ATechProjectile> Projectile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MeshSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SpawnSocketName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector WeaponRelativeLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator WeaponRelativeRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EnergyUsage;
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TECHBREACH_API UBaseWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBaseWeaponComponent();

// Actions
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	class UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	class UInputAction* FireAction;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void RequestFire();

// Weapon Management
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bWeaponIsActive;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bIsPlayerWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bAttachOnBeginPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FWeaponData CurrentWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TArray<FWeaponData> InstalledWeapons;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void InstallWeapon(FWeaponData Weapon);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void UninstallWeapon(uint8 WeaponIndex);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ActivateModule(uint8 NewSubmodCount);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	uint8 MaxInventorySize;

private:
	UFUNCTION()
	void AttachSubModule();

	UFUNCTION()
	bool CanFire();

	UFUNCTION()
	void ResetCanFire();

	UFUNCTION()
	void FireActionAoE();

	UFUNCTION()
	void FireActionProjectile();

	UFUNCTION()
	void FireActionInstantHit();

	UFUNCTION()
	void FireActionBeam();

	UPROPERTY()
	FTimerHandle TimerHandle_FireRate;

	UPROPERTY()
	bool bCanFire;

	UPROPERTY()
	USkeletalMeshComponent* WeaponMeshComponent;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
};
