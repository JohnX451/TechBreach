// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)


#include "BaseWeaponComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TechProjectile.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/PlayerController.h>
#include "PlayerBaseCharacter.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Particles/ParticleSystem.h"
#include "StatsComponent.h"
#include "Components/SphereComponent.h"

UBaseWeaponComponent::UBaseWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBaseWeaponComponent::RequestFire()
{
	if(!CanFire()) return;

	if (bIsPlayerWeapon)
	{
		UStatsComponent* StatsComponent = GetOwner()->FindComponentByClass<UStatsComponent>();
		if (StatsComponent)
		{
			StatsComponent->RemoveEnergy(CurrentWeapon.EnergyUsage);
		} else
		{
			UE_LOG(LogTemp, Error, TEXT("Weapon component: no player stats component found!"))
		}
	}
	
	bCanFire = false;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_FireRate,
		this,
		&UBaseWeaponComponent::Fired,
		CurrentWeapon.FireRate,
		false,
		CurrentWeapon.FireRate
		);

	FTransform SpawnTransform(
		WeaponMeshComponent->GetSocketRotation(CurrentWeapon.SpawnSocketName),
		WeaponMeshComponent->GetSocketLocation(CurrentWeapon.SpawnSocketName)
		);

	auto Projectile = Cast<ATechProjectile>(
		UGameplayStatics::BeginDeferredActorSpawnFromClass(
			this,
			CurrentWeapon.Projectile,
			SpawnTransform,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn,
			GetOwner())
			);

	Projectile->CollisionComponent->MoveIgnoreActors.Add(GetOwner());
	Projectile->Damage = CurrentWeapon.Damage;

	if (CurrentWeapon.FireEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(CurrentWeapon.FireEffect, WeaponMeshComponent, NAME_None, WeaponMeshComponent->GetSocketLocation(CurrentWeapon.SpawnSocketName), WeaponMeshComponent->GetSocketRotation(CurrentWeapon.SpawnSocketName), EAttachLocation::KeepWorldPosition);
	}
	
	UGameplayStatics::FinishSpawningActor(Projectile, SpawnTransform);
}

void UBaseWeaponComponent::InstallWeapon(FWeaponData Weapon)
{
	if(InstalledWeapons.Num() < MaxInventorySize)
	{
		InstalledWeapons.Add(Weapon);
		CurrentWeapon = Weapon;
		AttachSubModule();
		bCanFire = true;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Inventory Full, could not add weapon"));
	}
}

void UBaseWeaponComponent::UninstallWeapon(uint8 WeaponIndex)
{
	if(WeaponIndex < InstalledWeapons.Num())
		InstalledWeapons.RemoveAt(WeaponIndex);
}

void UBaseWeaponComponent::ActivateModule(uint8 NewSubmodCount)
{
	MaxInventorySize = NewSubmodCount;
}

void UBaseWeaponComponent::AttachSubModule()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, true);
	WeaponMeshComponent = NewObject<USkeletalMeshComponent>(Character, FName("Weapon"));
	WeaponMeshComponent->SetupAttachment(Character->GetMesh(), CurrentWeapon.MeshSocketName);
	WeaponMeshComponent->RegisterComponent();
	WeaponMeshComponent->SetSkeletalMesh(CurrentWeapon.WeaponMesh);
	WeaponMeshComponent->SetRelativeLocation(CurrentWeapon.WeaponRelativeLocation, false);
	WeaponMeshComponent->SetRelativeRotation(CurrentWeapon.WeaponRelativeRotation);
	bWeaponIsActive = true;
}

bool UBaseWeaponComponent::CanFire()
{
	if (UStatsComponent* StatsComponent = GetOwner()->FindComponentByClass<UStatsComponent>())
	{
		if (StatsComponent->GetCurrentEnergy() < CurrentWeapon.EnergyUsage)
		{
			return false;
		}
	}
	
	return bCanFire;
}

void UBaseWeaponComponent::Fired()
{
	bCanFire = true;
}

void UBaseWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (bAttachOnBeginPlay)
	{
		AttachSubModule();
		bCanFire = true;
	} else
	{
		bCanFire = false;
	}
	
	if (bIsPlayerWeapon)
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
			if (InputSubsystem)
			{
				InputSubsystem->AddMappingContext(InputMappingContext, 1);
			}
			UEnhancedInputComponent* InputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent);
			if (InputComponent)
			{
				InputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UBaseWeaponComponent::RequestFire);
			}
		}
	}
}

