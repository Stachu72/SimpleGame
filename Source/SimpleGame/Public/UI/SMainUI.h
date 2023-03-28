// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SMainUI.generated.h"

class ASCharacter;
class UHorizontalBox;
class UImage;
class UOverlay;
class UProgressBar;
class USItemSlot;

UCLASS()
class SIMPLEGAME_API USMainUI : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(Transient, VisibleDefaultsOnly)
	ASCharacter* PlayerClass;
	
	UPROPERTY(meta = (BindWidget))
	UImage* StatusBackground;


	
	/*********************************/
	/** Energy                      **/
	/*********************************/

public:
	void DecreasingEnergy();
	void StartIncreasingEnergy();

private:
	void IncreasingEnergy();

	UPROPERTY(EditDefaultsOnly, Category = "Energy")
	float IncreaseEnergyMultiplier = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Energy")
	float DecreaseEnergyMultiplier = 0.25f;
	UPROPERTY(EditDefaultsOnly, Category = "Energy")
	float IncreaseEnergyDelay = 2;
	
	FTimerHandle IncreaseEnergyTimer;

	/* Energy widget */
	UPROPERTY(meta = (BindWidget))
	UOverlay* EnergyBox;
	UPROPERTY(meta = (BindWidget))
	UImage* EnergyIcon;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* EnergyBar;


	
	/*********************************/
	/** Toolbar                     **/
	/*********************************/

public:
	void ScrollBetweenToolbarSlots(const float ScrollValue);
	
	void CheckPressedKey();

	void UpdateSlotIcon(const int32 SlotIndex, UTexture2D* NewIcon);

	void RemoveActiveSlotIcon() const;

	int32 GetActiveSlotIndex() const;
	
private:
	void InitializeToolbarSlotsArray();
	
	bool IsKeyPressed(const FKey Key) const { return GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(Key); }
	
	void UpdateActiveToolbarSlot(USItemSlot* NewActiveSlot);
	
	UPROPERTY(VisibleAnywhere, Category = "Toolbar")
	USItemSlot* ActiveSlot;

	TArray<USItemSlot*> ToolbarSlots;
	
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* ToolbarSlotsBox;
	
	UPROPERTY(meta = (BindWidget))
	USItemSlot* Slot_0;
	UPROPERTY(meta = (BindWidget))
	USItemSlot* Slot_1;
	UPROPERTY(meta = (BindWidget))
	USItemSlot* Slot_2;
	UPROPERTY(meta = (BindWidget))
	USItemSlot* Slot_3;
	UPROPERTY(meta = (BindWidget))
	USItemSlot* Slot_4;
	UPROPERTY(meta = (BindWidget))
	USItemSlot* Slot_5;
	UPROPERTY(meta = (BindWidget))
	USItemSlot* Slot_6;
	UPROPERTY(meta = (BindWidget))
	USItemSlot* Slot_7;
	UPROPERTY(meta = (BindWidget))
	USItemSlot* Slot_8;

	UPROPERTY(EditDefaultsOnly, Category = "Icons")
	UTexture2D* InActiveSlotIcon;
	UPROPERTY(EditDefaultsOnly, Category = "Icons")
	UTexture2D* ActiveSlotIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Icons")
	UTexture2D* EmptySlotIcon;
};
