// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SInventoryComponent.h"
#include "Items/SBaseItem.h"

// Sets default values for this component's properties
USInventoryComponent::USInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}

// Called when the game starts
void USInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	SlotsArray.Init(EmptySlot, SlotsCount);
	
}

bool USInventoryComponent::AddItem(ASBaseItem* ItemToAdd, int32 SlotIndex)
{
	if(!ItemToAdd)
	{
		return false;
	}

	const bool IsSlotEmpty = SlotsArray[SlotIndex] == EmptySlot;
	if(IsSlotEmpty)
	{
		SlotsArray[SlotIndex] = ItemToAdd;
		return true;
	}
	
	return false;
}

bool USInventoryComponent::PickupItem(ASBaseItem* ItemToAdd, int32& UsedSlot)
{
	if(!ItemToAdd)
	{
		return false;
	}

	//Check for an empty slot
	for (int32 i = 0; i < SlotsCount; i++)
	{
		const bool IsSlotEmpty = SlotsArray[i] == EmptySlot;
		if(!IsSlotEmpty)
		{
			continue;
		}
		
		//If found, add the item to the inventory
		if(AddItem(ItemToAdd, i))
		{
			UsedSlot = i;
			return true;
		}
	}
	
	return false;
}

bool USInventoryComponent::DropItem(const int32 ItemIndex)
{
	const bool IsSlotEmpty = SlotsArray[ItemIndex] == EmptySlot;
	if(IsSlotEmpty)
	{
		return false;
	}
	
	SpawnItem(ItemIndex);

	//Clear the item's slot
	SlotsArray[ItemIndex] = EmptySlot;
	
	return true;
}

void USInventoryComponent::SpawnItem(const int32 ItemIndex)
{
	const TSubclassOf<ASBaseItem> ItemToSpawn = SlotsArray[ItemIndex]->GetBaseItemStats().ClassRef;
	const FVector Location = GetOwner()->GetActorLocation() + FVector(0.f, 0.f, -2.1f); //Add the offset because a item should be under the player character
	const FRotator Rotation = FRotator::ZeroRotator;
	GetWorld()->SpawnActor<ASBaseItem>(ItemToSpawn, Location, Rotation);
}
