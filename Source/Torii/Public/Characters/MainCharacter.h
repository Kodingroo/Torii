/* TORII: Pixel 2D Platformer - GAMEDEV.TV Game Jam Project - Copyright Isaac Hayward 2022 */
#pragma once

#include "CoreMinimal.h"
#include "MainPlayerController.h"
#include "PaperCharacter.h"
#include "Gameplay/ToriiCamera.h"
#include "PaperFlipbook.h"
#include "WingsFlipbookComponent.h"
#include "MainCharacter.generated.h"

class UTextRenderComponent;

USTRUCT()
struct FInteractionData
{
	GENERATED_BODY()

	FInteractionData()
	{
		ViewedInteractionComponent = nullptr;
		LastInteractionCheckTime = 0.f;
		bInteractHeld = false;
	}

	//The current interactable component we're viewing, if there is one
	UPROPERTY()
	class UInteractionComponent* ViewedInteractionComponent;

	//The time when we last checked for an interactable
	UPROPERTY()
	float LastInteractionCheckTime;

	//Whether the local player is holding the interact key
	UPROPERTY()
	bool bInteractHeld;

};

UCLASS(config=Game)
class AMainCharacter : public APaperCharacter
{
	GENERATED_BODY()

public:
	AMainCharacter();

	UTextRenderComponent* TextComponent;
	bool IsWallSliding;
	float HitObjectDirection;
	float WallSlideDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Audio")
	USoundBase* JumpSoundCue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wings")
	UWingsFlipbookComponent* WingsFlipbook;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wings")
	bool FirstJump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp")
	bool OverlapLamp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Reference")
	AMainPlayerController* PlayerController;
	
	bool WallSlideCheck;;
	int OppositeFacingDirection;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room Camera")
	AToriiCamera* CurrentCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squish")
	float StartWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squish")
	float StartHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squish")
	bool IsMirrored;

	/* Create squished animation quality for jumping */
	void Squish(float ModWidth, float ModHeight, float SquishDuration );

	FTimerHandle WallSlidingHandle;
	FTimerHandle SquishHandle;

	// ---------- INTERACTION ---------- //
	//How often in seconds to check for an interactable object. Set this to zero if you want to check every tick.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionCheckFrequency;

	//How far we'll trace when we check if the player is looking at an interactable object
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionCheckDistance;

	void WallSlide(float Value);
	
	void PerformInteractionCheck();

	void CouldntFindInteractable();
	void FoundNewInteractable(UInteractionComponent* Interactable);

	void BeginInteract();
	void EndInteract();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBeginInteract();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEndInteract();

	void Interact();

	UFUNCTION()
	void OpenMenu();

	UPROPERTY(EditAnywhere, Category="Widgets")
	TSubclassOf<UUserWidget> MainMenuWidget;

	//Information about the current state of the players interaction
	UPROPERTY()
	FInteractionData InteractionData;

	//Helper function to make grabbing interactable faster
	FORCEINLINE class UInteractionComponent* GetInteractable() const { return InteractionData.ViewedInteractionComponent; }

	FTimerHandle TimerHandle_Interact;

	//True if we're interacting with an item that has an interaction time (for example a lamp that takes 2 seconds to turn on)
	bool IsInteracting() const;

	//Get the time till we interact with the current interactable
	float GetRemainingInteractTime() const;

	// ---------- PROPERTIES ---------- //

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Jump")
	int JumpCounter;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Jump")
	int MaximumJumps;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Jump")
	float JumpHeight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Dash")
	int DashCounter;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Dash")
	float DashDistance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Climb")
	mutable bool OnLadder;
	
protected:
	virtual void BeginPlay() override;
	
	// The animation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations)
	UPaperFlipbook* RunningAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UPaperFlipbook* IdleAnimation;
	
	// The animation to play while jupmoing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UPaperFlipbook* JumpingAnimation;
	
	// The animation to play while landing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UPaperFlipbook* LandingAnimation;
		
	// The animation to play while hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UPaperFlipbook* HitAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UPaperFlipbook* WingsAnimation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UPaperFlipbook* SlidingAnimation;
	

	/** Called to choose the correct animation to play based on the character's movement state */
	void UpdateAnimation();

	/** Called for side to side input */
	void MoveRight(float Value);

	UFUNCTION()
	void RightInput(float Value);

	/** Called for up/down ladder input */
	void MoveForward(float Value);

	void UpdateCharacter();

	/** Handle touch inputs. */
	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Handle touch stop event. */
	void TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	void DoubleJump();

	virtual void Landed(const FHitResult& Hit) override;

};

