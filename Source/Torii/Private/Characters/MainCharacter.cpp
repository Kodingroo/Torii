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

DEFINE_LOG_CATEGORY_STATIC(SideScrollerCharacter, Log, All);


AMainCharacter::AMainCharacter() :
	JumpCounter(0),
	MaximumJumps(1),
	JumpHeight(750.f),
	DashCounter(0),
	DashDistance(2000.f),
	HitObjectDirection(0.f),
	IsWallSliding(false),
	OnLadder(false),
	OverlapLamp(false)
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
	GetCharacterMovement()->AirControl = 0.5f;

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
	InteractionCheckDistance = 20.f;
	
	// WingsFlipbook->AddLocalTransform( );
	// WingsFlipbook->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	// WingsFlipbook->SetVisibility(true);
	// WingsFlipbook->SetLooping(false)
	
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
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::DoubleJump);
	// PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMainCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMainCharacter::TouchStopped);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMainCharacter::BeginInteract);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMainCharacter::EndInteract);
	PlayerInputComponent->BindAction("OpenMenu", IE_Pressed, this, &AMainCharacter::OpenMenu);
}

void AMainCharacter::OpenMenu()
{
	if (MainMenuWidget)
	{
		UUserWidget* MMW = Cast<UUserWidget>(CreateWidget(GetWorld(), MainMenuWidget));
		if (MMW)
		{
			MMW->AddToViewport();

			AMainPlayerController* PC = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)); 
			PC->bShowMouseCursor;
		}
	}
}

void AMainCharacter::DoubleJump()
{
	Squish(-.2, .2, .2);

	float JumpZ = GetCharacterMovement()->JumpZVelocity;

	UE_LOG(LogTemp, Warning, TEXT("Found Wall!"));
	if (GetCharacterMovement()->IsFalling() && IsWallSliding)
	{
			float AddedForce = -2;
			float OppositeFacingDirection = 0;
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

				// GEngine->AddOnScreenDebugMessage(-5, 5.f, FColor::Red,
				// 				 FString::Printf(TEXT("Wall Returns: %f"),
				// 					 GetSprite()->GetForwardVector().X ));
		UGameplayStatics::PlaySound2D(GetWorld(), JumpSoundCue);
	}
	else if (JumpCounter < MaximumJumps && !IsWallSliding)
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

void AMainCharacter::MoveRight(float Value)
{
	/*UpdateChar();*/

	if (!OnLadder)
	{
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
	}
	else
	{
		AddMovementInput(FVector(0.0f));
	}
	// Apply the input to the character motion

	WallSlide(Value);
}


void AMainCharacter::MoveForward(float Value)
{
	if(OnLadder)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		AddMovementInput(GetSprite()->GetUpVector(), Value);
	}
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

void AMainCharacter::WallSlide(float Value)
{
	FVector EyesLoc;
	FRotator EyesRot;
	FHitResult WallSlideTraceHit;

	GetController()->GetActorEyesViewPoint(EyesLoc, EyesRot);

	FVector TraceStart = EyesLoc + FVector(0.f, 0.f,-50.f);;
	FVector TraceEnd = (EyesRot.Vector() * 16.f) + TraceStart;
	
	// DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red );

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	IsWallSliding = false;
	
	if (GetWorld()->LineTraceSingleByChannel(WallSlideTraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		HitObjectDirection = WallSlideTraceHit.Location.X;
		//Check if we hit an interactable object

		if (WallSlideTraceHit.GetActor())
		{
			if (GetCharacterMovement()->IsFalling() && GetCharacterMovement()->Velocity.Z < 0 && Value != 0)
			{
				IsWallSliding = true;
				// GEngine->AddOnScreenDebugMessage(-5, 5.f, FColor::Red,
				// 				 FString::Printf(TEXT("Left Wall Returns: %f"),
				// 					 GetSprite()->GetForwardVector().X));
				//
				WallSlideDirection = WallSlideTraceHit.GetActor()->GetActorRotation().Yaw;
				
				GetCharacterMovement()->Velocity = FVector(0.f,0.f,-100.f);
			}
		}
	}
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
