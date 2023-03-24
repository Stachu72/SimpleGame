// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SMainUI.generated.h"

class ASCharacter;
class UImage;
class UOverlay;
class UProgressBar;

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
};
