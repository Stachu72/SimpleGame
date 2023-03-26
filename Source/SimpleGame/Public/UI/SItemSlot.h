// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SItemSlot.generated.h"

class UBorder;
class UImage;

UCLASS()
class SIMPLEGAME_API USItemSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	/* Setters */
	void SetBackground(UBorder* NewBackground) { Background = NewBackground; }
	void SetItemIcon(UImage* NewItemIcon) { ItemIcon = NewItemIcon; }

	/* Getters */
	UBorder* GetBackground() const { return Background; }
	UImage* GetItemIcon() const { return ItemIcon; }
	
private:
	UPROPERTY(meta = (BindWidget))
	UBorder* Background;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemIcon;
};
