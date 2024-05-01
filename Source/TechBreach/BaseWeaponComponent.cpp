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

// Sets default values for this component's properties
UBaseWeaponComponent::UBaseWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bCanFire = true;
}

void UBaseWeaponComponent::RequestFire()
{
	if(!CanFire())
		return;
	//ToDo: Reduce Energy
	bCanFire = false;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_FireRate, this, &UBaseWeaponComponent::Fired, CurrentWeapon.FireRate, false, CurrentWeapon.FireRate);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Firing"));
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<ATechProjectile>(CurrentWeapon.Projectile, WeaponMeshComponent->GetSocketLocation(FName("ProjectileSpawn")),WeaponMeshComponent->GetSocketRotation(FName("ProjectileSpawn")), ActorSpawnParameters);
}

void UBaseWeaponComponent::InstallWeapon(FWeaponData Weapon)
{
	if(InstalledWeapons.Num() < MaxInventorySize)
	{
		InstalledWeapons.Add(Weapon);
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

void UBaseWeaponComponent::ActivateModule()
{
	
}

void UBaseWeaponComponent::AttachSubModule()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, true);
	WeaponMeshComponent = NewObject<USkeletalMeshComponent>(Character, FName("Weapon"));
	WeaponMeshComponent->SetupAttachment(Character->GetRootComponent(), NAME_None);
	WeaponMeshComponent->RegisterComponent();
	WeaponMeshComponent->SetSkeletalMesh(CurrentWeapon.WeaponMesh);
	WeaponMeshComponent->SetRelativeLocation(FVector(63.0f, 0.f, 54.f), false);
	WeaponMeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
}

bool UBaseWeaponComponent::CanFire()
{
	// ToDo: Check if energy or weapon requirements met
	return bCanFire;
}

void UBaseWeaponComponent::Fired()
{
	bCanFire = true;
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Fired Called"));
}

// Called when the game starts
void UBaseWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (!bIsPlayerWeapon)
		return;

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

	AttachSubModule();
}

