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

	/* The current interactable component we're viewing, if there is one */
	UPROPERTY()
	class UInteractionComponent* ViewedInteractionComponent;

	/* The time when we last checked for an interactable */
	UPROPERTY()
	float LastInteractionCheckTime;

	/* Whether the local player is holding the interact key */
	UPROPERTY()
	bool bInteractHeld;
};

UCLASS(config=Game)
class AMainCharacter : public APaperCharacter
{
	GENERATED_BODY()

public:
	AMainCharacter();

	void WingsAnimationCheck();
	
	virtual void Tick(float DeltaSeconds) override;
	
	// ---------- PROPERTIES ---------- //

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Slide")
	bool WallSlideCheck;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Slide")
	float DisengageSlidingTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Jump")
	int WallJumpForce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Jump")
	int OppositeFacingDirection;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room Camera")
	UTextRenderComponent* TextComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room Camera")
	bool IsWallSliding;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room Camera")
	float HitObjectDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room Camera")
	float WallSlideDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room Camera")
	AToriiCamera* CurrentCamera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squish")
	float StartWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squish")
	float StartHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squish")
	bool IsMirrored;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Timers")
	FTimerHandle WallSlidingHandle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Timers")
	FTimerHandle SquishHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component | Wings")
	UPaperFlipbookComponent* WingsComponent;;
	
	// ---------- INTERACTION ---------- //
	
	//How often in seconds to check for an interactable object. Set this to zero if you want to check every tick.
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionCheckFrequency;
	//How far we'll trace when we check if the player is looking at an interactable object
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionCheckDistance;
	//True if we're interacting with an item that has an interaction time (for example a lamp that takes 2 seconds to turn on)
	bool IsInteracting() const;
	//Get the time till we interact with the current interactable
	float GetRemainingInteractTime() const;
	//Information about the current state of the players interaction
	UPROPERTY()
	FInteractionData InteractionData;
	UPROPERTY()
	FTimerHandle TimerHandle_Interact;
	
	UFUNCTION()
	void PerformInteractionCheck();
	UFUNCTION()
	void CouldntFindInteractable();
	UFUNCTION()
	void FoundNewInteractable(UInteractionComponent* Interactable);
	UFUNCTION()
	void BeginInteract();
	UFUNCTION()
	void EndInteract();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBeginInteract();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEndInteract();
	UFUNCTION()
	void Interact();
	
	/* Helper function to make grabbing interactable faster */
	FORCEINLINE class UInteractionComponent* GetInteractable() const { return InteractionData.ViewedInteractionComponent; }
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	
	// ---------- ANIMATIONS ---------- //
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations)
	UPaperFlipbook* RunningAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UPaperFlipbook* IdleAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UPaperFlipbook* JumpingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UPaperFlipbook* LandingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UPaperFlipbook* HitAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UPaperFlipbook* WingsAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UPaperFlipbook* SlidingAnimation;

	// ---------- FUNCTIONS ---------- //

	UFUNCTION()
	void WallSlide(float Value);
	/** Called to choose the correct animation to play based on the character's movement state */
	void UpdateAnimation();
	UFUNCTION()
	void UpdateCharacter();
	/** Evaluate with direction the player is facing and whether they are Wall Sliding, which requires an input delay */
	UFUNCTION()
	void MoveRight(float Value);
	/* Apply side to side input */ 
	UFUNCTION()
	void RightInput(float Value);
	/* Called for up/down ladder input */
	UFUNCTION()
	void MoveForward(float Value);
	/* Also called when Maximum Jump is still set to 1 */
	UFUNCTION()
	void DoubleJump();
	/* Create squished animation quality for jumping and landing */
	UFUNCTION()
	void Squish(float ModWidth, float ModHeight, float SquishDuration );
	
	virtual void Landed(const FHitResult& Hit) override;
};

