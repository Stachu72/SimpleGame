// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SMainUI.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "Player/SCharacter.h"

void USMainUI::NativeConstruct()
{
	Super::NativeConstruct();

	//Find and assign the player character class
	PlayerClass = Cast<ASCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
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
