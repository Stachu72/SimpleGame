// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable CppTooWideScope

#include "UI/SMainUI.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "Player/SCharacter.h"
#include "UI/SItemSlot.h"

void USMainUI::NativeConstruct()
{
	Super::NativeConstruct();

	//Find and assign the player character class
	PlayerClass = Cast<ASCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	InitializeToolbarSlotsArray();
	UpdateActiveToolbarSlot(ToolbarSlots[0]);
}

void USMainUI::DecreasingEnergy()
{
	if(!EnergyBar || !PlayerClass)
	{
		return;
	}

	//Stop filling up the energy bar
	if(GetWorld()->GetTimerManager().IsTimerActive(IncreaseEnergyTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(IncreaseEnergyTimer);
	}

	/* Decrease energy until it reach 0 */
	const float EnergyAmount = EnergyBar->GetPercent();
	const bool IsEnergyEmpty = EnergyAmount <= 0.f;
	if(IsEnergyEmpty)
	{
		const float WalkSpeed = PlayerClass->GetWalkSpeed();
		PlayerClass->UpdateMovementSpeed(WalkSpeed);
		PlayerClass->ReleaseSprintButton();
		
		StartIncreasingEnergy();
		
		return;
	}

	const float DecreaseAmount = GetWorld()->GetDeltaSeconds() * DecreaseEnergyMultiplier;
	EnergyBar->SetPercent(EnergyAmount - DecreaseAmount);
}

void USMainUI::StartIncreasingEnergy()
{
	if(!EnergyBar || !PlayerClass)
	{
		return;
	}

	//Check if the timer is not already active
	if(GetWorld()->GetTimerManager().IsTimerActive(IncreaseEnergyTimer))
	{
		return;
	}

	//After the delay, start increasing the energy bar
	GetWorld()->GetTimerManager().SetTimer(IncreaseEnergyTimer, this, &USMainUI::IncreasingEnergy, GetWorld()->GetDeltaSeconds(), true, IncreaseEnergyDelay);
}

void USMainUI::IncreasingEnergy()
{
	if(!EnergyBar || !PlayerClass)
	{
		GetWorld()->GetTimerManager().ClearTimer(IncreaseEnergyTimer);
		return;
	}

	/* Fill the energy bar until it reaches maximum or if the player makes a sprint */
	const float EnergyAmount = EnergyBar->GetPercent();
	const bool IsEnergyFull = EnergyAmount >= 1.f;
	if(PlayerClass->IsPlayerSprinting() || IsEnergyFull)
	{
		GetWorld()->GetTimerManager().ClearTimer(IncreaseEnergyTimer);
		
		return;
	}

	const float IncreaseAmount = GetWorld()->GetDeltaSeconds() * IncreaseEnergyMultiplier;
	EnergyBar->SetPercent(EnergyAmount + IncreaseAmount);
}

void USMainUI::InitializeToolbarSlotsArray()
{
	//Add all toolbar slots to one array
	ToolbarSlots.Add(Slot_0);
	ToolbarSlots.Add(Slot_1);
	ToolbarSlots.Add(Slot_2);
	ToolbarSlots.Add(Slot_3);
	ToolbarSlots.Add(Slot_4);
	ToolbarSlots.Add(Slot_5);
	ToolbarSlots.Add(Slot_6);
	ToolbarSlots.Add(Slot_7);
	ToolbarSlots.Add(Slot_8);
}

void USMainUI::ScrollBetweenToolbarSlots(const float ScrollValue)
{
	/* Scroll down -> Swipe active slot to left */
	const bool bScrollingDown = ScrollValue < 0;
	if(bScrollingDown)
	{
		//Check which slot is active
		for(int i = 0; i < ToolbarSlots.Num(); i++)
		{
			const bool IsSlotActive = ToolbarSlots[i] == ActiveSlot;
			if(IsSlotActive)
			{
				const bool IsFirstSlotActive = i == 0;
				if(IsFirstSlotActive)
				{
					USItemSlot* LastItemSlot = ToolbarSlots[ToolbarSlots.Num() - 1];
					UpdateActiveToolbarSlot(LastItemSlot);
					
					return;
				}

				USItemSlot* PreviousSlot = ToolbarSlots[i-1];
				UpdateActiveToolbarSlot(PreviousSlot);
				
				return;
			}
		}
	}
	
	/* Scroll up -> Swipe active slot to right */
	const bool bScrollingUp = ScrollValue > 0;
	if(bScrollingUp)
	{
		//Check which slot is active
		for(int i = 0; i < ToolbarSlots.Num(); i++)
		{
			const bool IsSlotActive = ToolbarSlots[i] == ActiveSlot;
			if(IsSlotActive)
			{
				const bool IsLastSlotActive = i == ToolbarSlots.Num() - 1;
				if(IsLastSlotActive)
				{
					USItemSlot* FirstSlot = ToolbarSlots[0];
					UpdateActiveToolbarSlot(FirstSlot);
					
					return;
				}

				USItemSlot* NextSlot = ToolbarSlots[i+1];
				UpdateActiveToolbarSlot(NextSlot);
				
				return;
			}
		}
	}
}

void USMainUI::CheckPressedKey()
{
	if(IsKeyPressed(EKeys::One))
	{
		UpdateActiveToolbarSlot(Slot_0);
	}
	else if(IsKeyPressed(EKeys::Two))
	{
		UpdateActiveToolbarSlot(Slot_1);
	}
	else if(IsKeyPressed(EKeys::Three))
	{
		UpdateActiveToolbarSlot(Slot_2);
	}
	else if(IsKeyPressed(EKeys::Four))
	{
		UpdateActiveToolbarSlot(Slot_3);
	}
	else if(IsKeyPressed(EKeys::Five))
	{
		UpdateActiveToolbarSlot(Slot_4);
	}
	else if(IsKeyPressed(EKeys::Six))
	{
		UpdateActiveToolbarSlot(Slot_5);
	}
	else if(IsKeyPressed(EKeys::Seven))
	{
		UpdateActiveToolbarSlot(Slot_6);
	}
	else if(IsKeyPressed(EKeys::Eight))
	{
		UpdateActiveToolbarSlot(Slot_7);
	}
	else if(IsKeyPressed(EKeys::Nine))
	{
		UpdateActiveToolbarSlot(Slot_8);
	}
}

void USMainUI::UpdateActiveToolbarSlot(USItemSlot* NewActiveSlot)
{
	const bool IsSlotAlreadyActive = NewActiveSlot == ActiveSlot;
	if(IsSlotAlreadyActive)
	{
		return;
	}
	
	const bool IsActiveSlotSet = ActiveSlot != nullptr;
	if(IsActiveSlotSet)
	{
		/* Inactive the old active slot */
		UBorder* BackgroundHandle = ActiveSlot->GetBackground();
		BackgroundHandle->SetBrushFromTexture(InActiveSlotIcon);
		BackgroundHandle->SetBrushColor(FColor::FromHex(TEXT("FFFFFFFF")));
		ActiveSlot->SetBackground(BackgroundHandle);
		BackgroundHandle->SetPadding(FMargin(15.f, 15.f, 15.f, 25.f));
	}
	
	if(!NewActiveSlot)
	{
		return;
	}

	ActiveSlot = NewActiveSlot;
	
	/* Active the new active slot */
	UBorder* BackgroundHandle = NewActiveSlot->GetBackground();
	BackgroundHandle->SetBrushFromTexture(ActiveSlotIcon);
	BackgroundHandle->SetBrushColor(FColor::FromHex(TEXT("E1D7D1FF")));
	NewActiveSlot->SetBackground(BackgroundHandle);
	BackgroundHandle->SetPadding(FMargin(15.f, 20.f, 15.f, 20.f));
}

void USMainUI::UpdateSlotIcon(const int32 SlotIndex, UTexture2D* NewIcon)
{
	UImage* ImageHandle = ToolbarSlots[SlotIndex]->GetItemIcon();
	ImageHandle->SetBrushFromTexture(NewIcon);
}

void USMainUI::RemoveActiveSlotIcon() const
{
	UImage* ImageHandle = ActiveSlot->GetItemIcon();
	ImageHandle->SetBrushFromTexture(EmptySlotIcon);
}

int32 USMainUI::GetActiveSlotIndex() const
{
	if(!ActiveSlot)
	{
		return 0;
	}

	return ToolbarSlots.Find(ActiveSlot);
}
