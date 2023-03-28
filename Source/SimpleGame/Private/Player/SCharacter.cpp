// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "UI/SMainUI.h"
#include "PaperFlipbookComponent.h"
#include "Components/SInventoryComponent.h"
#include "Items/SBaseItem.h"

ASCharacter::ASCharacter()
{
	ConfigurePlayerComponents();
}

void ASCharacter::ConfigurePlayerComponents()
{
	//Configure the player collision
	GetCapsuleComponent()->InitCapsuleSize(11.f, 20.f);
	
	//Configures the player mesh
	GetSprite()->SetRelativeLocation(FVector(0.f, -5.f, -19.8f));
	GetSprite()->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));
	
	//Configure the player camera boom
	CameraBoomComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoomComp->SetupAttachment(RootComponent);
	CameraBoomComp->SetRelativeRotation(FRotator(-90.f, -90.f, 0.f));
	CameraBoomComp->TargetArmLength = 120.f;
	CameraBoomComp->bDoCollisionTest = false;

	//Configure the player camera
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(CameraBoomComp);

	//Configure the player movement component
	GetCharacterMovement()->MaxWalkSpeed = 50.f;

	//Set default flipbook
	CharacterDirection = Down;
	SetMoveAnimation();

	//Set player inventory
	InventoryComp = CreateDefaultSubobject<USInventoryComponent>(TEXT("Inventory"));
}

void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	ConfigureMappingContext();

	CreateMainUI();
}

void ASCharacter::ConfigureMappingContext() const
{
	//Make sure that we have a valid PlayerController
	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		//Get the Enhanced Input Local Player Subsystem from the Local Player related to our Player Controller
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			//PawnClientRestart can run more than once in an Actor's lifetime, so start by clearing out any leftover mappings
			Subsystem->ClearAllMappings();

			//Add each mapping context, along with their priority values. Higher values out prioritize lower values
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
}

void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Make sure that we are using a UEnhancedInputComponent; if not, the project is not configured correctly
	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//This calls the handler function on the tick when MyInputAction starts, such as when pressing an action button
		if(MovementAction)
		{
			PlayerEnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ASCharacter::MovePlayer);
			PlayerEnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Completed, this, &ASCharacter::SetIdleAnimation);
		}

		if(SprintAction)
		{
			PlayerEnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ASCharacter::StartSprinting);
			PlayerEnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ASCharacter::StopSprinting);
		}

		//Scrolling between toolbar slots
		if(ScrollingToolbarAction)
		{
			PlayerEnhancedInputComponent->BindAction(ScrollingToolbarAction, ETriggerEvent::Started, this, &ASCharacter::StartScrollingToolbar);
		}

		//Press a specified button to pick up an item
		if(PickupItemAction)
		{
			PlayerEnhancedInputComponent->BindAction(PickupItemAction, ETriggerEvent::Started, this, &ASCharacter::TryToPickupItem);
		}

		//Press a specified button to drop an item
		if(DropItemAction)
		{
			PlayerEnhancedInputComponent->BindAction(DropItemAction, ETriggerEvent::Started, this, &ASCharacter::TryToDropItem);
		}
		
		//Press 1-9 key, to active a toolbar slot
		PlayerInputComponent->BindKey(EKeys::One, IE_Pressed, this, &ASCharacter::OnPressedToolbarSlotKey);
		PlayerInputComponent->BindKey(EKeys::Two, IE_Pressed, this, &ASCharacter::OnPressedToolbarSlotKey);
		PlayerInputComponent->BindKey(EKeys::Three, IE_Pressed, this, &ASCharacter::OnPressedToolbarSlotKey);
		PlayerInputComponent->BindKey(EKeys::Four, IE_Pressed, this, &ASCharacter::OnPressedToolbarSlotKey);
		PlayerInputComponent->BindKey(EKeys::Five, IE_Pressed, this, &ASCharacter::OnPressedToolbarSlotKey);
		PlayerInputComponent->BindKey(EKeys::Six, IE_Pressed, this, &ASCharacter::OnPressedToolbarSlotKey);
		PlayerInputComponent->BindKey(EKeys::Seven, IE_Pressed, this, &ASCharacter::OnPressedToolbarSlotKey);
		PlayerInputComponent->BindKey(EKeys::Eight, IE_Pressed, this, &ASCharacter::OnPressedToolbarSlotKey);
		PlayerInputComponent->BindKey(EKeys::Nine, IE_Pressed, this, &ASCharacter::OnPressedToolbarSlotKey);
	}
}

void ASCharacter::MovePlayer(const FInputActionValue& ActionValue)
{
	const FVector2d InputValue = ActionValue.Get<FVector2d>(); 

	//Move the player vertical
	AddMovementInput(FVector(0.f, -1.f, 0.f) * InputValue.Y);
	//Move the player horizontal
	AddMovementInput(FVector(1.f, 0.f, 0.f) * InputValue.X);

	CalculateDirection(InputValue);
	SetMoveAnimation();
}

void ASCharacter::StartSprinting(const FInputActionValue& ActionValue)
{
	IsSprintKeyPressed = true;

	UpdateMovementSpeed(SprintSpeed);

	//Decrease energy during the player sprinting 
	GetWorld()->GetTimerManager().SetTimer(CheckMoveTimer, this, &ASCharacter::CheckForPlayerMove, GetWorld()->GetDeltaSeconds(), true);
}

void ASCharacter::CheckForPlayerMove()
{
	if(!MainUIClass)
	{
		GetWorld()->GetTimerManager().ClearTimer(CheckMoveTimer);
		return;
	}

	//Wait for the player move
	const bool IsMoving = GetVelocity().Length() >= 0.01f;
	if(IsMoving && IsSprintKeyPressed)
	{
		MainUIClass->DecreasingEnergy();
	}
	else if(!IsMoving)
	{
		MainUIClass->StartIncreasingEnergy();
	}
	else if(!IsSprintKeyPressed)
	{
		GetWorld()->GetTimerManager().ClearTimer(CheckMoveTimer);
	}
}

void ASCharacter::StopSprinting(const FInputActionValue& ActionValue)
{
	IsSprintKeyPressed = false;

	MainUIClass->StartIncreasingEnergy();
	
	UpdateMovementSpeed(WalkSpeed);
}

void ASCharacter::UpdateMovementSpeed(const float SpeedValue) const
{
	//Update the player's flipbook and speed
	GetCharacterMovement()->MaxWalkSpeed = SpeedValue;
	GetSprite()->SetPlayRate(SpeedValue * 0.02f);
}

void ASCharacter::CalculateDirection(const FVector2d Value)
{
	//Check which direction the player is facing
	if(FMath::Abs(Value.X) > FMath::Abs(Value.Y))
	{
		if(Value.X < 0.f)
		{
			CharacterDirection = Left;
		}
		else
		{
			CharacterDirection = Right;
		}
	}
	else
	{
		if(Value.Y > 0.f)
		{
			CharacterDirection = Up;
		}
		else
		{
			CharacterDirection = Down;
		}
	}
}

void ASCharacter::SetMoveAnimation() const
{
	//Set a move flipbook based on the character direction
	switch (CharacterDirection)
	{
	case Up:
		if(!Flipbooks.MoveUp)
		{
			return;
		}
		
		GetSprite()->SetFlipbook(Flipbooks.MoveUp);
		break;

	case Right:
		if(!Flipbooks.MoveRight)
		{
			return;
		}
		
		GetSprite()->SetFlipbook(Flipbooks.MoveRight);
		break;
		
	case Down:
		if(!Flipbooks.MoveDown)
		{
			return;
		}
		
		GetSprite()->SetFlipbook(Flipbooks.MoveDown);
		break;

	case Left:
		if(!Flipbooks.MoveLeft)
		{
			return;
		}
			
		GetSprite()->SetFlipbook(Flipbooks.MoveLeft);
		break;
	}
}

void ASCharacter::SetIdleAnimation()
{
	//Set a idle flipbook based on the character direction
	switch (CharacterDirection)
	{
	case Up:
		if(!Flipbooks.IdleUp)
		{
			return;
		}
		
		GetSprite()->SetFlipbook(Flipbooks.IdleUp);
		break;

	case Right:
		if(!Flipbooks.IdleRight)
		{
			return;
		}
		
		GetSprite()->SetFlipbook(Flipbooks.IdleRight);
		break;
		
	case Down:
		if(!Flipbooks.IdleDown)
		{
			return;
		}
		
		GetSprite()->SetFlipbook(Flipbooks.IdleDown);
		break;

	case Left:
		if(!Flipbooks.IdleLeft)
		{
			return;
		}
			
		GetSprite()->SetFlipbook(Flipbooks.IdleLeft);
		break;
	}
}

void ASCharacter::CreateMainUI()
{
	MainUIClass = Cast<USMainUI>(CreateWidget(GetWorld(), TSubMainUI));

	if(MainUIClass)
	{
		MainUIClass->AddToViewport();
	}
}

void ASCharacter::StartScrollingToolbar(const FInputActionValue& ActionValue)
{
	if(MainUIClass)
	{
		const float ScrollValue = ActionValue.Get<float>();
		MainUIClass->ScrollBetweenToolbarSlots(ScrollValue);
	}
}

void ASCharacter::OnPressedToolbarSlotKey()
{
	if(MainUIClass)
	{
		MainUIClass->CheckPressedKey();
	}
}

void ASCharacter::TryToPickupItem()
{
	//Get information about the overlapping actors
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, TSubclassOf<ASBaseItem>());

	for (AActor* OverlappedActor : OverlappingActors)
	{
		//Check for overlapped with a item
		ASBaseItem* OverlappedItem = Cast<ASBaseItem>(OverlappedActor);
		if(!OverlappedItem)
		{
			continue;
		}

		/* Pickup the item and update a slot icon */
		int32 UsedSlot = 0;
		if(InventoryComp->PickupItem(OverlappedItem, UsedSlot))
		{
			if(!MainUIClass)
			{
				return;
			}

			UTexture2D* ItemIcon = OverlappedItem->GetBaseItemStats().Icon;
			MainUIClass->UpdateSlotIcon(UsedSlot, ItemIcon);

			OverlappedActor->Destroy();
		
			break;
		}
	}
}

void ASCharacter::TryToDropItem()
{
	if(!MainUIClass)
	{
		return;
	}

	/* Drop the item and update a slot icon */
	const int32 SlotIndex = MainUIClass->GetActiveSlotIndex();
	if(InventoryComp->DropItem(SlotIndex))
	{
		MainUIClass->RemoveActiveSlotIcon();
	}
}
