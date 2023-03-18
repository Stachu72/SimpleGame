// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBaseCharacter.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UPaperFlipbook;
class USpringArmComponent;

struct FInputActionValue;

UENUM(Category = "Animation")
enum EAnimationDirection
{
	Up,
	Right,
	Down,
	Left
};

USTRUCT(BlueprintType, Category = "Animation")
struct FFlipbooksAnimations
{
	GENERATED_BODY()
	
	/** Idle flipbooks **/
	UPROPERTY(EditDefaultsOnly)
	UPaperFlipbook* IdleUp;

	UPROPERTY(EditDefaultsOnly)
	UPaperFlipbook* IdleRight;

	UPROPERTY(EditDefaultsOnly)
	UPaperFlipbook* IdleDown;

	UPROPERTY(EditDefaultsOnly)
	UPaperFlipbook* IdleLeft;

	/** Movement flipbooks **/
	UPROPERTY(EditDefaultsOnly)
	UPaperFlipbook* MoveUp;

	UPROPERTY(EditDefaultsOnly)
	UPaperFlipbook* MoveRight;

	UPROPERTY(EditDefaultsOnly)
	UPaperFlipbook* MoveDown;

	UPROPERTY(EditDefaultsOnly)
	UPaperFlipbook* MoveLeft;
};

UCLASS()
class SIMPLEGAME_API ASCharacter : public ASBaseCharacter
{
	GENERATED_BODY()

public:
	//Sets defaults value
	ASCharacter();

private:
	//Called when character is spawned
	virtual void BeginPlay() override;
	
	void ConfigurePlayerComponents();

	/* Boom to handle distance between the camera and the player */
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* CameraBoomComp;
	
	/* Primary camera of the player */
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* CameraComp;
	
	

	/**********************************/
	/* Input                          */
	/**********************************/
	
	void ConfigureMappingContext() const;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MovementAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* MappingContext;
	
	
	
	/*********************************/
	/** Movement                    **/
	/*********************************/

	void MovePlayer(const FInputActionValue& ActionValue);

	void CalculateDirection(const FVector2d Value);

	void SetMoveAnimation() const;
	void SetIdleAnimation();
	
	EAnimationDirection CharacterDirection;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Animations")
	FFlipbooksAnimations Flipbooks;
	
};
