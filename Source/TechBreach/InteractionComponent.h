// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TECHBREACH_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionComponent();

	UFUNCTION()
	void Interact();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Maximum distance from screen position in cm to interact with objects
	UPROPERTY(EditAnywhere)
	float Reach = 180.0f;
	
	// Actor that is currently being recognized as interactive
	UPROPERTY(EditAnywhere)
	AActor* CurrentlyHighlightedActor;

	// Mesh component that was last highlighted
	UPROPERTY()
	UMeshComponent* LastHighlightedComponent;

	// Timer handle used to continuously trace for actors to highlight
	UPROPERTY()
	FTimerHandle TimerHandle_Trace;

	// Show debug trace and log messages
	UPROPERTY(EditAnywhere)
	bool bDebug = false;

	UFUNCTION()
	void TraceAction();

	UFUNCTION()
	static void SetHighlight(AActor* Interactee, bool bIsHighlighted);

private:
	// Return hit for first physics body in reach
	UFUNCTION()
	FHitResult GetFirstWorldDynamicInReach();

	// Returns current start of reach line
	UFUNCTION()
	FVector GetReachLineStart() const;

	// Returns current end of reach line
	UFUNCTION()
	FVector GetReachLineEnd(float Distance) const;
};
