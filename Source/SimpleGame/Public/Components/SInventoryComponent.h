////// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SInventoryComponent.generated.h"

class ASBaseItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIMPLEGAME_API USInventoryComponent final : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USInventoryComponent();

	bool AddItem(ASBaseItem* ItemToAdd, int32 SlotIndex);
	bool PickupItem(ASBaseItem* ItemToAdd, int32& UsedSlot);

	bool DropItem(const int32 ItemIndex);
	
	/* Setters */
	void SetSlotsCount(const int32 NewSlotsCount) { SlotsCount = NewSlotsCount; }
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	void SpawnItem(const int32 ItemIndex);
	
	UPROPERTY(VisibleAnywhere, Category = "Slots")
	TArray<ASBaseItem*> SlotsArray;

	UPROPERTY(EditDefaultsOnly, Category = "Slots")
	ASBaseItem* EmptySlot;

	UPROPERTY(EditAnywhere, Category = "Slots")
	int32 SlotsCount = 9;
};
