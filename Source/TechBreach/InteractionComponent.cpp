// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)


#include "InteractionComponent.h"
#include "Interactable.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Math/Vector.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


void UInteractionComponent::Interact()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Player pressed E")));
	if (CurrentlyHighlightedActor)
	{
		Cast<IInteractable>(CurrentlyHighlightedActor)->Execute_Use(CurrentlyHighlightedActor);
	}
}

// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Trace, this, &UInteractionComponent::TraceAction, 0.05f, true);
}

void UInteractionComponent::TraceAction()
{
	const auto HitResult = GetFirstWorldDynamicInReach();

	if (const auto HitActor = HitResult.GetActor())
	{
		if (HitActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()) && HitActor != CurrentlyHighlightedActor)
		{
			SetHighlight(CurrentlyHighlightedActor, false);
			CurrentlyHighlightedActor = HitActor;
			SetHighlight(CurrentlyHighlightedActor, true);
		} else if (HitActor != CurrentlyHighlightedActor)
		{
			SetHighlight(CurrentlyHighlightedActor, false);
			CurrentlyHighlightedActor = nullptr;
		}
	}
	else {
		if (!HitActor && CurrentlyHighlightedActor)
		{
			SetHighlight(CurrentlyHighlightedActor, false);
			CurrentlyHighlightedActor = nullptr;
		}
	}
}

void UInteractionComponent::SetHighlight(AActor* Interactee, bool bIsHighlighted)
{
	if (Interactee) {
		Cast<IInteractable>(Interactee)->Execute_SetInteractionPopup(Interactee, bIsHighlighted);
		TArray<UActorComponent*> HighlightableComponents = Interactee->GetComponentsByTag(UMeshComponent::StaticClass(), FName("highlightable"));
		if (HighlightableComponents.IsEmpty()) return;
		for (UActorComponent* MeshComp : HighlightableComponents)
		{
			Cast<UMeshComponent>(MeshComp)->SetRenderCustomDepth(bIsHighlighted);
		}
	}
}

FHitResult UInteractionComponent::GetFirstWorldDynamicInReach()
{
	if (bDebug) DrawDebugLine(GetWorld(), GetReachLineStart(), GetReachLineEnd(Reach), FColor(255,0,0,64), false, 0.2f, 0, 2.0f);

	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	FHitResult Hit;

	// Defining channels which can be traced
	FCollisionObjectQueryParams TraceCollisionChannels;
	TraceCollisionChannels.AddObjectTypesToQuery(ECC_WorldDynamic);
	TraceCollisionChannels.AddObjectTypesToQuery(ECC_WorldStatic);

	GetWorld()->LineTraceSingleByObjectType(OUT Hit, GetReachLineStart(), GetReachLineEnd(Reach), FCollisionObjectQueryParams(TraceCollisionChannels), TraceParameters);

	return Hit;
}

FVector UInteractionComponent::GetReachLineStart() const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);

	return PlayerViewPointLocation;
}

FVector UInteractionComponent::GetReachLineEnd(float Distance) const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);

	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Distance;
}
