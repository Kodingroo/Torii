/* TORII: Pixel 2D Platformer - GAMEDEV.TV Game Jam Project - Copyright Isaac Hayward 2022 */

#include "Characters/MainCharacter.h"

#include "DrawDebugHelpers.h"
#include "PaperFlipbookComponent.h"
#include "Characters/MainPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Core/Debug.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/GameModeBase.h"
#include "Gameplay/Components/InteractionComponent.h"
#include "Kismet/GameplayStatics.h"

// DEFINE_LOG_CATEGORY_STATIC(SideScrollerCharacter, Log, All);


AMainCharacter::AMainCharacter() :
	JumpCounter(0),
	MaximumJumps(1),
	JumpHeight(750.f),
	DashCounter(0),
	DashDistance(2000.f),
	HitObjectDirection(0.f),
	IsWallSliding(false),
	OnLadder(false),
	OverlapLamp(false),
	WallSlideCheck(false)
{
	/* Use only Yaw from the controller and ignore the rest of the rotation. */
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	/* Set the size of the collision capsule. */
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f);
	GetCapsuleComponent()->SetCapsuleRadius(40.0f);
	
	GetCharacterMovement()->bOrientRotationToMovement = false;

	/* Configure character movement */
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->MaxFlySpeed = 600.0f;
	GetCharacterMovement()->AirControl = 0.5f;

	/* Lock character motion onto the XZ plane */
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

	/* Enable replication on the Sprite component so animations show up when networked */
	GetSprite()->SetIsReplicated(true);
	bReplicates = true;

	/* Set how often and at what distance the Player Character should check for Scene objects with Interactive Components attached */ 
	InteractionCheckFrequency = 0.f;
	InteractionCheckDistance = 20.f;
	
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	/* Starting Scale values are assigned here for use with the Squish Animation effect */
	StartWidth = GetSprite()->GetRelativeScale3D().X;
	StartHeight = GetSprite()->GetRelativeScale3D().Z;
}

void AMainCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	/* Updating the animation of the Character as well as i's facing direction */
	UpdateCharacter();
	
	if (GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}
}

// -------------------- INPUT -------------------- //

void AMainCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	/* Note: the 'Jump' action and the 'MoveRight' axis are bound to actual keys/buttons/sticks in DefaultInput.ini (editable from Project Settings..Input) */
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::DoubleJump);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMainCharacter::BeginInteract);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMainCharacter::EndInteract);
	PlayerInputComponent->BindAction("OpenMenu", IE_Pressed, this, &AMainCharacter::OpenMenu);
}

void AMainCharacter::MoveRight(float Value)
{
	WallSlide(Value);
	
	if( !GetCharacterMovement()->IsFalling() || !IsWallSliding )
	{
		WallSlideCheck = false; 
		AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
	}
	else if( IsWallSliding && WallSlideCheck == false)
	{
		WallSlideCheck = true;
		GetWorld()->GetTimerManager().ClearTimer(WallSlidingHandle);
		const float FacingValue = GetSprite()->GetForwardVector().X ? 1.f : -1.f;
		const FTimerDelegate SlidingDelegate = FTimerDelegate::CreateUObject( this, &AMainCharacter::RightInput, FacingValue );
		GetWorld()->GetTimerManager().SetTimer(WallSlidingHandle, SlidingDelegate, .6f, true);
		// GetWorld()->GetTimerManager().SetTimer(WallSlidingHandle, [&]()
		// {
		// 	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), FacingValue);
		// }, .7f, false);
	}
}

void AMainCharacter::RightInput(float Value)
{
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}

void AMainCharacter::MoveForward(float Value)
{
	if(OnLadder)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		AddMovementInput(GetSprite()->GetUpVector(), Value);
	}
}

void AMainCharacter::OpenMenu()
{
	if (MainMenuWidget)
	{
		UUserWidget* MainMenu = Cast<UUserWidget>(CreateWidget(GetWorld(), MainMenuWidget));
		if (MainMenu)
		{
			MainMenu->AddToViewport();

			PlayerController = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)); 
			PlayerController->bShowMouseCursor;
		}
	}
}

// -------------------- CHARACTER MOVEMENT -------------------- //

void AMainCharacter::DoubleJump()
{
	Squish(-.2, .2, .2);

	float JumpZ = GetCharacterMovement()->JumpZVelocity;
	
	if (GetCharacterMovement()->IsFalling() && IsWallSliding)
	{
			const float AddedForce = -2;
			OppositeFacingDirection = 0;
			if (GetSprite()->GetForwardVector().X < 0)
			{
				OppositeFacingDirection = GetActorRotation().Yaw  ;
				LaunchCharacter(FVector(OppositeFacingDirection * AddedForce, 0.f, JumpZ), true, true);
			}
			else if (GetSprite()->GetForwardVector().X > 0)
			{
				OppositeFacingDirection = GetActorRotation().Yaw + 180.f;
				LaunchCharacter(FVector(OppositeFacingDirection * AddedForce, 0.f, JumpZ), true, true);
			}
			
			IsWallSliding = false;
		
		UGameplayStatics::PlaySound2D(GetWorld(), JumpSoundCue);
	}
	else if (JumpCounter < MaximumJumps)
	{
		ACharacter::LaunchCharacter(FVector(0.f,0.f,JumpZ), false,true);
		JumpCounter++;

		UGameplayStatics::PlaySound2D(GetWorld(), JumpSoundCue);
	}
	if (MaximumJumps > 1)
	{
		
	}
	// UDebug::Print(WARNING, "Current Max Jumps: " + FString::FromInt(MaximumJumps));
}

void AMainCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	
	Squish(.3, -.3, 0.1);

	/* Reset the ability to Jump and Dash when landed */
	JumpCounter = 0;
	DashCounter = 0;
}

void AMainCharacter::WallSlide(float Value)
{
	FVector EyesLoc;
	FRotator EyesRot;
	FHitResult WallSlideTraceHit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	/* It's important to use ActorEyesViewpoint and not PlayerViewPoint as the character itself needs to find Interactable objects from its perspective */ 
	GetController()->GetActorEyesViewPoint(EyesLoc, EyesRot);

	FVector TraceStart = EyesLoc + FVector(0.f, 0.f,-50.f);;
	FVector TraceEnd = (EyesRot.Vector() * 16.f) + TraceStart;

	/* Used to see Interaction Distance */ 
	// DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red );
	
	if (GetWorld()->LineTraceSingleByChannel(WallSlideTraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		if (WallSlideTraceHit.GetActor() && !WallSlideTraceHit.GetActor()->GetName().Contains("Lamp")
			&& GetCharacterMovement()->IsFalling() && GetCharacterMovement()->Velocity.Z < 0 && Value != 0)
		{
			IsWallSliding = true;

			WallSlideDirection = WallSlideTraceHit.GetActor()->GetActorRotation().Yaw;
				
			GetCharacterMovement()->Velocity = FVector(0.f,0.f,-100.f);
		}
	}
	else if( WallSlideTraceHit.GetActor() == nullptr)
	{
		IsWallSliding = false;
	}
}

void AMainCharacter::UpdateCharacter()
{
	/* Update animation to match the motion */
	UpdateAnimation();

	/* Setup the rotation of the controller based on the direction we are travelling */
	const FVector PlayerVelocity = GetVelocity();
	const float TravelDirection = PlayerVelocity.X;
	
	/* Set the rotation so that the character faces his direction of travel. */
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
	FHitResult TraceHit;

	GetController()->GetActorEyesViewPoint(EyesLoc, EyesRot);

	FVector TraceStart = EyesLoc + FVector(0.f, 0.f,-50.f);;
	FVector TraceEnd = (EyesRot.Vector() * InteractionCheckDistance) + TraceStart;
	
	// DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red );

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);


	if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		// UDebug::Print(WARNING, "Found Actor");
		//Check if we hit an interactable object
		if (TraceHit.GetActor())
		{
			if (UInteractionComponent* InteractionComponent = Cast<UInteractionComponent>(TraceHit.GetActor()->GetComponentByClass(UInteractionComponent::StaticClass())))
			{
				// UDebug::Print(WARNING, "Found an Interactable Actor");

				float Distance = (TraceStart - TraceHit.ImpactPoint).Size();
				if (InteractionComponent != GetInteractable() && Distance <= InteractionComponent->InteractionDistance)
				{
					// GEngine->AddOnScreenDebugMessage(-5, 2, FColor::Red, TEXT("HIT THE OBJECT"));
					// UE_LOG(LogTemp, Warning, TEXT("Found an interactable object!"));

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
	// UDebug::Print(WARNING, "Interact");		

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
		// UDebug::Print(ERROR, "Begin Interact");

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
	// GEngine->AddOnScreenDebugMessage(-5, 2, FColor::Red, TEXT("HIT THE OBJECT"));
	UE_LOG(LogTemp, Warning, TEXT("Found an interactable object!"));

	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->Interact(this);
		// GEngine->AddOnScreenDebugMessage(-5, 2, FColor::Red, TEXT("HIT THE OBJECT"));
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

// -------------------- ANIMATION -------------------- //

void AMainCharacter::Squish(float ModWidth, float ModHeight, float SquishDuration)
{
	if (GetCharacterMovement()->IsFalling() && !GetCharacterMovement()->GroundFriction)
	{
		GetSprite()->SetRelativeScale3D(FVector(StartWidth + ModWidth, 0.f, StartHeight + ModHeight));
	}
	else if ( GetCharacterMovement()->GroundFriction )
	{
		GetSprite()->SetRelativeScale3D(FVector(StartWidth + ModWidth, 0.f, StartHeight + ModHeight));
		GetWorld()->GetTimerManager().SetTimer(SquishHandle, [&]()
		{
			GetSprite()->SetRelativeScale3D(FVector(StartWidth, 0.f, StartHeight));
		}, SquishDuration, false);	
	}
}

void AMainCharacter::UpdateAnimation()
{
	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();

	/*  Is the character moving or standing still? */
	UPaperFlipbook* DesiredAnimation = (PlayerSpeedSqr > 0.0f) ? RunningAnimation : IdleAnimation;
	
	if( GetSprite()->GetFlipbook() != DesiredAnimation && !GetCharacterMovement()->IsFalling())
	{
		GetSprite()->SetFlipbook(DesiredAnimation);
	}
	else if( GetCharacterMovement()->IsFalling() && !IsWallSliding )
	{
		GetSprite()->SetFlipbook(JumpingAnimation);
	}
	else if( GetCharacterMovement()->IsFalling() && IsWallSliding )
	{
		GetSprite()->SetFlipbook(SlidingAnimation);
	}
	
}
