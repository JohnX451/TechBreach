// Copyright Fabian Lukas, Gessner Christian, Hofer Nadia, Klammer Arthur (2024-2025)


#include "InteractableItem.h"
#include "Components/StaticMeshComponent.h"

AInteractableItem::AInteractableItem()
{
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	Mesh->ComponentTags.Add("highlightable");
}

void AInteractableItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AInteractableItem::Use_Implementation()
{
	IInteractable::Use_Implementation();
}

void AInteractableItem::SetInteractionPopup_Implementation(bool bShow)
{
	IInteractable::SetInteractionPopup_Implementation(bShow);
}


