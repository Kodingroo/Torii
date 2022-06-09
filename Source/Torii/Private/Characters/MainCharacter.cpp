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
	WallDisengageRequestCheck(false),
	DisengageSlidingTimer(0.6f),
	WallJumpForce(-2),
	OppositeFacingDirection(0)
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

	/* The Wings Animation was created as a separate animation to apply to the Character Animation */ 
	WingsComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Wings"));
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	/* Starting Scale values are assigned here for use with the Squish Animation effect */
	StartWidth = GetSprite()->GetRelativeScale3D().X;
	StartHeight = GetSprite()->GetRelativeScale3D().Z;

	WingsComponent->SetFlipbook(WingsAnimation);
	WingsComponent->SetLooping(true);
	WingsComponent->SetVisibility(true);

	/* Establish References */
	PlayerController = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)); 
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

	/* Because the Wings Animation was added separately from character animations, Relative position must be Ticked */ 
	WingsAnimationCheck();
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
}

void AMainCharacter::MoveRight(float Value)
{
	/* Determine if the Character has engaged a Wall or not */
	WallSlide(Value);

	/* Used to compare with the Player Input to see if they wish to move in the opposite direction from which the Character engaged the Wall  */
	float CharacterFacingValue;
	if (GetSprite()->GetForwardVector().X <= 0) { CharacterFacingValue = -1.f; } else { CharacterFacingValue = 1.f; }

	/* Movement for all instances but Wall Jumping */
	if( !GetCharacterMovement()->IsFalling() || !IsWallSliding )
	{
		WallDisengageRequestCheck = false; 
		AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
	}
	/* Determine how long to disable Controller Input before disengaging from the Wall. This allows for leeway when a Player is trying to Wall Jump */
	else if( IsWallSliding && WallDisengageRequestCheck == false && Value != CharacterFacingValue )
	{
		/* Must prevent the Tick from resetting the Timer while sliding */
		WallDisengageRequestCheck = true;
		
		GetWorld()->GetTimerManager().ClearTimer(WallSlidingHandle);
		GetWorld()->GetTimerManager().SetTimer(WallSlidingHandle, [&]()
		{
			/* A -1.f/ 1.f value must be applied to AddMovementInput otherwise the Character does not move away from the wall */
			float DetachWallValue;
			if (GetActorForwardVector().X > 0 ) { DetachWallValue = -1.f; } else { DetachWallValue = 1.f; }
			AddMovementInput(FVector(1.0f, 0.0f, 0.0f), DetachWallValue);

			/* Ensure delay is being called when the Player is pushing away from the wall */ 
			// UDebug::PrintToScreen(WARNING, "Calling Delay", 1.f);
		}, DisengageSlidingTimer, false);	
	}
}

void AMainCharacter::MoveForward(float Value)
{
	if(OnLadder)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		AddMovementInput(GetSprite()->GetUpVector(), Value);
	}
}

// -------------------- CHARACTER MOVEMENT -------------------- //

void AMainCharacter::DoubleJump()
{
	const float JumpZ = GetCharacterMovement()->JumpZVelocity;

	WingsComponent->SetVisibility(false);

	if (JumpCounter == 0)
	{
		/* Add squishy feel to the Character Animation on first Jump key press */
		Squish(-.2, .2, .2);
	}
	
	if (GetCharacterMovement()->IsFalling() && IsWallSliding)
	{
			/* Determine Wall Jump direction based on the direction the Character engaged the wall */
			if (GetSprite()->GetForwardVector().X < 0)
			{
				OppositeFacingDirection = GetActorRotation().Yaw  ;
				LaunchCharacter(FVector(OppositeFacingDirection * WallJumpForce, 0.f, JumpZ), true, true);
			}
			else if (GetSprite()->GetForwardVector().X > 0)
			{
				OppositeFacingDirection = GetActorRotation().Yaw + 180.f;
				LaunchCharacter(FVector(OppositeFacingDirection * WallJumpForce, 0.f, JumpZ), true, true);
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
	
	if (JumpCounter > 1)
	{
		WingsComponent->SetVisibility(true);
	}

	/* Check Jump values are working as expected */
	// UDebug::Print(WARNING, "Current Jump Counter : " + FString::FromInt(JumpCounter) + " / Current Max Jumps: " + FString::FromInt(MaximumJumps));
}

void AMainCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	WingsComponent->SetVisibility(false);

	/* Add squishy feel to the Character Animation when Landing */
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

	/* It's important to use ActorEyesViewpoint and not PlayerViewPoint as the character itself needs to find Hit Actors from its perspective */ 
	GetController()->GetActorEyesViewPoint(EyesLoc, EyesRot);

	FVector TraceStart = EyesLoc + FVector(0.f, 0.f,-50.f);;
	FVector TraceEnd = (EyesRot.Vector() * 16.f) + TraceStart;

	/* Used to see Interaction Distance */ 
	// DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red );
	
	if (GetWorld()->LineTraceSingleByChannel(WallSlideTraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		/* Contains("Lamp") required to prevent odd sliding glitch when Character overlaps the Lamp's Mesh Box */ 
		if (WallSlideTraceHit.GetActor() && !WallSlideTraceHit.GetActor()->GetName().Contains("Lamp")
			&& GetCharacterMovement()->IsFalling() && GetCharacterMovement()->Velocity.Z < 0 && Value != 0)
		{
			WingsComponent->SetVisibility(false);
			
			IsWallSliding = true;
			
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
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	/* It's important to use ActorEyesViewpoint and not PlayerViewPoint as the character itself needs to find Interactable Objects from its perspective */ 
	GetController()->GetActorEyesViewPoint(EyesLoc, EyesRot);

	FVector TraceStart = EyesLoc + FVector(0.f, 0.f,-50.f);;
	FVector TraceEnd = (EyesRot.Vector() * InteractionCheckDistance) + TraceStart;

	/* Determine the Interaction distance of the Character */
	// DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red );
	
	if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		if (TraceHit.GetActor())
		{
			/* Check if we hit an Actor with an Interactable Component attached */
			if (UInteractionComponent* InteractionComponent = Cast<UInteractionComponent>(TraceHit.GetActor()->GetComponentByClass(UInteractionComponent::StaticClass())))
			{
				float Distance = (TraceStart - TraceHit.ImpactPoint).Size();
				
				if (InteractionComponent != GetInteractable() && Distance <= InteractionComponent->InteractionDistance)
				{
					/* Check if an Interactable Object has been collided with */ 
					// UDebug::PrintToScreen(WARNING, "Hit an Interactable Object", 1 );

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
	/* Lost focus on an interactable so clear the timer. */
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_Interact))
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interact);
	}

	/* Tell the interactable we've stopped focusing on it, and clear the current interactable */
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
	In this case, the server will check every tick for the duration of the interact */
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
	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->Interact(this);

		/* Check an Interactable Object was found */
		// UDebug::PrintToScreen(WARNING, "Found an Interactable Object!", 1.f );
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

void AMainCharacter::WingsAnimationCheck()
{
	if (GetActorForwardVector().X < 0 )
	{
		WingsComponent->SetRelativeLocation(GetActorLocation() + FVector(4.f, -0.1f, 10.f));
	}
	else
	{
		WingsComponent->SetRelativeLocation(GetActorLocation() + FVector(-4.f, -0.1f, 10.f));
	}

	/* Set Visibility set true in DoubleJump() and False in WallSlide() and Landed() */
}
