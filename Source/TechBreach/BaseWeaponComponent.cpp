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
#include "DrawDebugHelpers.h"

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

	switch (CurrentWeapon.FireMode)
	{
	case EFireMode::AreaOfEffect:
		FireActionAoE();
		break;
	case EFireMode::Projectile:
		FireActionProjectile();
		break;
	case EFireMode::InstantHit:
		FireActionInstantHit();
		break;
	case EFireMode::Beam:
		FireActionBeam();
		break;
	}

	Fired(CurrentWeapon);

	// ToDo: replace with attached audio component
	if (CurrentWeapon.FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), CurrentWeapon.FireSound, WeaponMeshComponent->GetSocketLocation(CurrentWeapon.SpawnSocketName), WeaponMeshComponent->GetSocketRotation(CurrentWeapon.SpawnSocketName));
	}
}

void UBaseWeaponComponent::RequestSwitchWeapon(const FInputActionValue& Value)
{
	if (InstalledWeapons.Num() <= 1 || !bCanSwitchWeapon) return;

	bCanFire = false;
	bCanSwitchWeapon = false;
	
	if (Value.Get<float>() > 0.f)
	{
		CurrentWeaponIndex = (CurrentWeaponIndex == InstalledWeapons.Num() - 1) ? 0 : CurrentWeaponIndex + 1;
	}
	else
	{
		CurrentWeaponIndex = (CurrentWeaponIndex == 0) ? InstalledWeapons.Num() - 1 : CurrentWeaponIndex - 1;
	}

	BeginWeaponSwitch();
	
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_SwitchWeaponCooldown,
		this,
		&UBaseWeaponComponent::ResetSwitchWeapon,
		0.5f,
		false,
		0.5f
	);

}

void UBaseWeaponComponent::InstallWeapon(FWeaponData Weapon)
{
	if(InstalledWeapons.Num() < MaxInventorySize)
	{
		InstalledWeapons.Add(Weapon);
		CurrentWeapon = Weapon;
		CurrentWeaponIndex = InstalledWeapons.Num() - 1;
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

bool UBaseWeaponComponent::PointWeaponForward()
{
	return bCanSwitchWeapon && bWeaponIsActive;
}

void UBaseWeaponComponent::HandleImplantUninstall()
{
	InstalledWeapons.Empty();
	CurrentWeaponIndex = 0;
	bCanFire = false;
	bWeaponIsActive = false;
	bCanSwitchWeapon = true;
	WeaponMeshComponent->SetVisibility(false);
}

void UBaseWeaponComponent::AttachSubModule()
{
	if (!WeaponMeshComponent)
	{
		ACharacter* Character = Cast<ACharacter>(GetOwner());
		FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, true);
		WeaponMeshComponent = NewObject<USkeletalMeshComponent>(Character, FName("Weapon"));
		WeaponMeshComponent->SetupAttachment(Character->GetMesh(), CurrentWeapon.MeshSocketName);
		WeaponMeshComponent->RegisterComponent();
	} else
	{
		WeaponMeshComponent->SetVisibility(true);
	}
	
	WeaponMeshComponent->SetSkeletalMesh(CurrentWeapon.WeaponMesh);
	WeaponMeshComponent->SetRelativeLocation(CurrentWeapon.WeaponRelativeLocation, false);
	WeaponMeshComponent->SetRelativeRotation(CurrentWeapon.WeaponRelativeRotation);
	WeaponMeshComponent->SetWorldScale3D(CurrentWeapon.WeaponScale);

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

void UBaseWeaponComponent::ResetCanFire()
{
	if (InstalledWeapons.Num() > 1)
	{
		if (bCanSwitchWeapon) bCanFire = true;
	}
	else
	{
		bCanFire = true;
	}
}

void UBaseWeaponComponent::FireActionAoE()
{
	bCanFire = false;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_FireRate,
		this,
		&UBaseWeaponComponent::ResetCanFire,
		CurrentWeapon.FireRate,
		false,
		CurrentWeapon.FireRate
	);
	
	FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(CurrentWeapon.AoEData.RadiusCm);
	
	TArray<FHitResult> OutResults;
	
	FVector WeaponOrigin = WeaponMeshComponent->GetSocketLocation(CurrentWeapon.SpawnSocketName);
	FRotator WeaponDirection = WeaponMeshComponent->GetSocketRotation(CurrentWeapon.SpawnSocketName);
	// The sphere needs to move at least a little bit, otherwise the trace doesn't work
	FVector WeaponTarget = WeaponOrigin + (WeaponDirection.Vector() * 1.f);

	//float DebugAngle = FMath::DegreesToRadians(CurrentWeapon.AoEData.AngleDegrees);
	//DrawDebugCone(GetWorld(), WeaponOrigin, WeaponDirection.Vector().GetSafeNormal(), CurrentWeapon.AoEData.RadiusCm, DebugAngle, DebugAngle, 16, FColor::Red, false, 1.f);

	FCollisionObjectQueryParams QueryParams = FCollisionObjectQueryParams();
	QueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	QueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	QueryParams.AddObjectTypesToQuery(ECC_Pawn);
	QueryParams.AddObjectTypesToQuery(ECC_Destructible);
	QueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);
	
	GetWorld()->SweepMultiByObjectType(OutResults, WeaponOrigin, WeaponTarget, WeaponDirection.Quaternion(), QueryParams, CollisionSphere);
	//GetWorld()->SweepMultiByProfile(OutResults, WeaponOrigin, WeaponTarget, WeaponDirection.Quaternion(), FName("Projectile"), CollisionSphere);
	//GetWorld()->SweepMultiByChannel(OutResults, WeaponOrigin, WeaponTarget, WeaponDirection.Quaternion(), ECollisionChannel::ECC_WorldDynamic, CollisionSphere);

	if (CurrentWeapon.FireEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(CurrentWeapon.FireEffect, WeaponMeshComponent, NAME_None, WeaponMeshComponent->GetSocketLocation(CurrentWeapon.SpawnSocketName), WeaponMeshComponent->GetSocketRotation(CurrentWeapon.SpawnSocketName), EAttachLocation::KeepWorldPosition);
	}
	
	TArray<AActor*> HitActors;
	
	for (auto Hit : OutResults)
	{
		if(!Hit.bBlockingHit) break;
		
		if(Hit.GetComponent() && Hit.GetComponent() == WeaponMeshComponent) break;
		
		if(Hit.GetActor() && (Hit.GetActor() != GetOwner()))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Pulse hit %s"), *Hit.GetActor()->GetName()));

			// Check if actor is within view cone
			FVector ConeTargetVector = Hit.ImpactPoint - WeaponOrigin;
			float ConeDotProduct = FVector::DotProduct(ConeTargetVector.GetSafeNormal(), WeaponDirection.Vector().GetSafeNormal());
			if(ConeDotProduct < cos(FMath::DegreesToRadians(CurrentWeapon.AoEData.AngleDegrees))) break;
			
			if (CurrentWeapon.AoEData.ImpactEffectDefault)
			{
				FRotator HitOrientation = (Hit.Normal - WeaponOrigin).GetSafeNormal().Rotation();
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CurrentWeapon.AoEData.ImpactEffectDefault, Hit.ImpactPoint, HitOrientation);
			}

			if (CurrentWeapon.AoEData.ImpactSoundDefault)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), CurrentWeapon.AoEData.ImpactSoundDefault, Hit.Location, Hit.ImpactNormal.Rotation());
			}

			if (Cast<ACharacter>(Hit.GetActor()))
			{
				// do some damage stuff
				if (!HitActors.Contains(Hit.GetActor()))
				{
					HitActors.Add(Hit.GetActor());
					float FinalDamage = CurrentWeapon.Damage;
					if (Hit.Distance > CurrentWeapon.AoEData.RadiusInnerCm)
					{
						FinalDamage = FMath::GetMappedRangeValueClamped(
							FVector2D(CurrentWeapon.AoEData.RadiusInnerCm, CurrentWeapon.AoEData.RadiusCm),
							FVector2D(FinalDamage, 0.f),
							Hit.Distance
							);
					}
					UGameplayStatics::ApplyPointDamage(Hit.GetActor(), FinalDamage, Hit.ImpactNormal, Hit, GetOwner()->GetInstigatorController(), GetOwner(), CurrentWeapon.AoEData.DamageType);
				}
				
			}

		}
	}
}

void UBaseWeaponComponent::FireActionProjectile()
{
	bCanFire = false;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_FireRate,
		this,
		&UBaseWeaponComponent::ResetCanFire,
		CurrentWeapon.FireRate,
		false,
		CurrentWeapon.FireRate
		);
	
	FTransform SpawnTransform(
		WeaponMeshComponent->GetSocketRotation(CurrentWeapon.SpawnSocketName),
		WeaponMeshComponent->GetSocketLocation(CurrentWeapon.SpawnSocketName)
		);

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Passing projectile instigator %s"), *GetOwner()->GetName()));
	
	auto Projectile = Cast<ATechProjectile>(
		UGameplayStatics::BeginDeferredActorSpawnFromClass(
			GetOwner(),
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

void UBaseWeaponComponent::FireActionInstantHit()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("BaseWeaponComponent: FireActionInstantHit() not implemented."));
}

void UBaseWeaponComponent::FireActionBeam()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("BaseWeaponComponent: FireActionBeam() not implemented."));
}

void UBaseWeaponComponent::ResetSwitchWeapon()
{
	CurrentWeapon = InstalledWeapons[CurrentWeaponIndex];
	AttachSubModule();
	bCanFire = true;
	bCanSwitchWeapon = true;
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
			CurrentWeaponIndex = 0;
			bCanSwitchWeapon = true;
			UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
			if (InputSubsystem)
			{
				InputSubsystem->AddMappingContext(InputMappingContext, 1);
			}
			UEnhancedInputComponent* InputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent);
			if (InputComponent)
			{
				InputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UBaseWeaponComponent::RequestFire);
				InputComponent->BindAction(SwitchWeaponAction, ETriggerEvent::Triggered, this, &UBaseWeaponComponent::RequestSwitchWeapon);
			}
		}
	}
}

