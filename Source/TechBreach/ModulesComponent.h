// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatsComponent.h"
#include "BaseWeaponComponent.h"
#include "ModulesComponent.generated.h"

UENUM(BlueprintType)
enum class EImplantSlot : uint8
{
	Head1,
	Head2,
	LeftArm,
	Legs,
	RightArm1,
	RightArm2,
	Torso1,
	Torso2
};

UENUM(BlueprintType)
enum class EImplantType : uint8
{
	Head,
	LeftArm,
	Legs,
	RightArm,
	Torso
};

USTRUCT(BlueprintType)
struct FSubmoduleData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponData WeaponData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EAttributeType, float> AttributeCoefTerms;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> CompatibleImplantIds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ImageUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Description;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InstallTimeSeconds;
};

USTRUCT(BlueprintType)
struct FImplantData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EAttributeType, float> AttributeCoefTerms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSubmoduleData> InstalledSubmodules;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* ImplantMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ImageUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Description;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InstallTimeSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 MaxSubmodules;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EImplantType ImplantType;
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TECHBREACH_API UModulesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UModulesComponent();

protected:
	/** The actor's active implants */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats)
	TMap<EImplantSlot, FImplantData> ActiveImplants;
	
	/** The actor's inactive implants */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats)
	TArray<FImplantData> InactiveImplants;

	/** The actor's inactive submodules */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats)
	TArray<FSubmoduleData> InactiveSubmodules;

public:
	/** Attempts to add the implant to the specified slot, returns true on success */
	UFUNCTION(BlueprintCallable)
	bool AddImplantToSlot(FImplantData Implant, EImplantSlot Slot);

	/** Attempts to add the submodule to the specified implant, returns true on success */
	UFUNCTION(BlueprintCallable)
	bool AddSubmoduleToImplant(FSubmoduleData Submodule, EImplantSlot Slot);

	/** Attempts to replace the submodule specified by implant slot and index with the new submodule */
	UFUNCTION(BlueprintCallable)
	bool ReplaceSubmoduleAtIndex(FSubmoduleData NewSubmodule, EImplantSlot Slot, int Index);

	/** Attempts to find and return a map of compatible installed implant modules */
	UFUNCTION(BlueprintCallable)
	bool FindCompatibleImplantsById(TArray<FName> CompatibleIds, TMap<EImplantSlot, FImplantData>& OutCompatibleImplants);

private:
	UPROPERTY()
	UStatsComponent* Stats;
	
	UFUNCTION()
	bool IsSlotFree(EImplantSlot Slot) const;

	UFUNCTION()
	static bool IsImplantCompatible(FSubmoduleData Submodule, FImplantData RequestedImplant);

	UFUNCTION()
	TMap<EAttributeType, float> CalculateAttributeCoefficients() const;

	UFUNCTION()
	void AttachSkeletalMeshToPlayer(FImplantData Implant);

	UFUNCTION()
	void ProcessWeaponSubmodule(FSubmoduleData Submodule, bool bRemove, uint8 WeaponIndex);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
};
