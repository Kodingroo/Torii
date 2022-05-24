// Copyright Isaac Lloyd Hayward. All Rights Reserved.

#include "Characters/MainCharacter.h"

#include "DrawDebugHelpers.h"
#include "PaperFlipbookComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Gameplay/Components/InteractionComponent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(SideScrollerCharacter, Log, All);


AMainCharacter::AMainCharacter()
{
	// Use only Yaw from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f);
	GetCapsuleComponent()->SetCapsuleRadius(40.0f);
	
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Configure character movement
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->MaxFlySpeed = 600.0f;
	// GetCharacterMovement()->FallingLateralFriction = 0.2f;

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

    // 	TextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarGear"));
    // 	TextComponent->SetRelativeScale3D(FVector(3.0f, 3.0f, 3.0f));
    // 	TextComponent->SetRelativeLocation(FVector(35.0f, 5.0f, 20.0f));
    // 	TextComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
    // 	TextComponent->SetupAttachment(RootComponent);

	// Enable replication on the Sprite component so animations show up when networked
	GetSprite()->SetIsReplicated(true);
	bReplicates = true;

	/* Interactions */ 
	InteractionCheckFrequency = 0.f;
	InteractionCheckDistance = 50.f;
}

//////////////////////////////////////////////////////////////////////////
// Animation

void AMainCharacter::Squish(float ModWidth, float ModHeight, float SquishDuration)
{
	if (GetCharacterMovement()->IsFalling() && !GetCharacterMovement()->GroundFriction)
	{
		GetSprite()->SetRelativeScale3D(FVector(StartWidth + ModWidth, 0.f, StartHeight + ModHeight));
	}
	else
		{
			GetSprite()->SetRelativeScale3D(FVector(StartWidth + ModWidth, 0.f, StartHeight + ModHeight));
			GetWorld()->GetTimerManager().SetTimer(SquishHandle, [&]()
			{
				GetSprite()->SetRelativeScale3D(FVector(StartWidth, 0.f, StartHeight));
			}, SquishDuration, false);	
		}
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	StartWidth = GetSprite()->GetRelativeScale3D().X;
	StartHeight = GetSprite()->GetRelativeScale3D().Z;
}

void AMainCharacter::UpdateAnimation()
{
	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();

	// Are we moving or standing still?
	UPaperFlipbook* DesiredAnimation = (PlayerSpeedSqr > 0.0f) ? RunningAnimation : IdleAnimation;
	
	if( GetSprite()->GetFlipbook() != DesiredAnimation && !GetCharacterMovement()->IsFalling())
	{
		GetSprite()->SetFlipbook(DesiredAnimation);
	}
	else if( GetCharacterMovement()->IsFalling())
	{
		GetSprite()->SetFlipbook(JumpingAnimation);
	}
}

void AMainCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	UpdateCharacter();

	if (GetCharacterMovement()->IsFalling())
	{
		FHitResult TraceHit;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		
		FVector SpriteLocation = GetSprite()->GetRelativeLocation();
		FVector SpriteForwardVector = GetSprite()->GetForwardVector() * FVector(70.f) + SpriteLocation;
		bool HittingWall = GetWorld()->LineTraceSingleByChannel(TraceHit, SpriteLocation, SpriteForwardVector, ECC_Visibility, QueryParams);

		if (HittingWall)
		{
			GEngine->AddOnScreenDebugMessage(-5, 5.f, FColor::Red, TEXT("Found a wall"));
			float WallSlideDirection = TraceHit.GetActor()->GetActorRotation().Yaw + 180.f;
			GetSprite()->SetRelativeRotation(FRotator(WallSlideDirection, 0.f, 0.f));
		}
		GetCharacterMovement()->Velocity = FMath::VInterpConstantTo(GetCharacterMovement()->Velocity, FVector(0.f), GetWorld()->GetDeltaSeconds(), 900.f);
	}

	if (GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}

}


//////////////////////////////////////////////////////////////////////////
// Input

void AMainCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Note: the 'Jump' action and the 'MoveRight' axis are bound to actual keys/buttons/sticks in DefaultInput.ini (editable from Project Settings..Input)
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMainCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMainCharacter::TouchStopped);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMainCharacter::BeginInteract);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMainCharacter::EndInteract);
}

void AMainCharacter::Jump()
{
	Super::Jump();
	
	Squish(-.2, .2, .2);
}

void AMainCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	
	Squish(.3, -.3, 0.1);
}

void AMainCharacter::MoveRight(float Value)
{
	/*UpdateChar();*/

	// Apply the input to the character motion
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}

void AMainCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Jump on any touch
	Jump();
}

void AMainCharacter::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Cease jumping once touch stopped
	StopJumping();
}

void AMainCharacter::UpdateCharacter()
{
	// Update animation to match the motion
	UpdateAnimation();

	// Now setup the rotation of the controller based on the direction we are travelling
	const FVector PlayerVelocity = GetVelocity();	
	float TravelDirection = PlayerVelocity.X;
	// Set the rotation so that the character faces his direction of travel.
	if (Controller != nullptr)
	{
		if (TravelDirection < 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0, 180.0f, 0.0f));
		}
		else if (TravelDirection > 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
	}
}

// -------------------- INTERACTING -------------------- //

bool AMainCharacter::IsInteracting() const
{
	return GetWorldTimerManager().IsTimerActive(TimerHandle_Interact);
}

float AMainCharacter::GetRemainingInteractTime() const
{
	return GetWorldTimerManager().GetTimerRemaining(TimerHandle_Interact);
}

void AMainCharacter::PerformInteractionCheck()
{
	if (GetController() == nullptr)
	{
		return;
	}
	
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector EyesLoc;
	FRotator EyesRot;

	GetController()->GetActorEyesViewPoint(EyesLoc, EyesRot);

	FVector TraceStart = EyesLoc + FVector(0.f, 0.f,-50.f);;
	FVector TraceEnd = (EyesRot.Vector() * InteractionCheckDistance) + TraceStart;
	FHitResult TraceHit;
	
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red );

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		//Check if we hit an interactable object
		if (TraceHit.GetActor())
		{
			if (UInteractionComponent* InteractionComponent = Cast<UInteractionComponent>(TraceHit.GetActor()->GetComponentByClass(UInteractionComponent::StaticClass())))
			{
				float Distance = (TraceStart - TraceHit.ImpactPoint).Size();
				if (InteractionComponent != GetInteractable() && Distance <= InteractionComponent->InteractionDistance)
				{
					FoundNewInteractable(InteractionComponent);
				}
				else if (Distance > InteractionComponent->InteractionDistance && GetInteractable())
				{
					CouldntFindInteractable();
				}

				return;
			}
		}
	}

	CouldntFindInteractable();

}

void AMainCharacter::CouldntFindInteractable()
{
	//We've lost focus on an interactable. Clear the timer.
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_Interact))
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interact);
	}

	//Tell the interactable we've stopped focusing on it, and clear the current interactable
	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->EndFocus(this);

		if (InteractionData.bInteractHeld)
		{
			EndInteract();
		}
	}

	InteractionData.ViewedInteractionComponent = nullptr;
}

void AMainCharacter::FoundNewInteractable(UInteractionComponent* Interactable)
{
	UE_LOG(LogTemp,Warning, TEXT("Found Interactable yeah!"));

	EndInteract();

	if (UInteractionComponent* OldInteractable = GetInteractable())
	{
		OldInteractable->EndFocus(this);
	}

	InteractionData.ViewedInteractionComponent = Interactable;
	Interactable->BeginFocus(this);

}

void AMainCharacter::BeginInteract()
{
	if (!HasAuthority())
	{
		ServerBeginInteract();
	}

	/**As an optimization, the server only checks that we're looking at an item once we begin interacting with it.
	This saves the server doing a check every tick for an interactable Item. The exception is a non-instant interact.
	In this case, the server will check every tick for the duration of the interact*/
	if (HasAuthority())
	{
		PerformInteractionCheck();
	}

	InteractionData.bInteractHeld = true;

	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->BeginInteract(this);

		if (FMath::IsNearlyZero(Interactable->InteractionTime))
		{
			Interact();
		}
		else
		{
			GetWorldTimerManager().SetTimer(TimerHandle_Interact, this, &AMainCharacter::Interact, Interactable->InteractionTime, false);
		}
	}
}

void AMainCharacter::EndInteract()
{
	if (!HasAuthority())
	{
		ServerEndInteract();
	}

	InteractionData.bInteractHeld = false;

	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->EndInteract(this);
	}
}

void AMainCharacter::Interact()
{
	GEngine->AddOnScreenDebugMessage(-5, 2, FColor::Red, TEXT("HIT THE OBJECT"));
	UE_LOG(LogTemp, Warning, TEXT("Found an interactable object!"));

	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->Interact(this);
		GEngine->AddOnScreenDebugMessage(-5, 2, FColor::Red, TEXT("HIT THE OBJECT"));
	}
}

void AMainCharacter::ServerEndInteract_Implementation()
{
	EndInteract();
}

bool AMainCharacter::ServerEndInteract_Validate()
{
	return true;
}

void AMainCharacter::ServerBeginInteract_Implementation()
{
	BeginInteract();
}

bool AMainCharacter::ServerBeginInteract_Validate()
{
	return true;
}
