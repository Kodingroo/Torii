/* TORII: Pixel 2D Platformer - GAMEDEV.TV Game Jam Project - Copyright Isaac Hayward 2022 */

#include "Characters/MainPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Characters/MainCharacter.h"
#include "Core/Debug.h"
#include "Core/ToriiGameMode.h"
#include "Gameplay/Feather.h"
#include "Kismet/GameplayStatics.h"


AMainPlayerController::AMainPlayerController() :
	TotalFeathers(0),
	CollectedFeathers(0)
{
	
}

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	GameModeRef = Cast<AToriiGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
}

void AMainPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindAction("OpenMenu", IE_Pressed, this, &AMainPlayerController::OpenMenu);
	}
}

void AMainPlayerController::OpenMenu()
{
	// UDebug::Print(WARNING, "Open Menu in Controller ");

	UUserWidget* MainMenu = CreateWidget(this, MainMenuWidget);
	if (MainMenu != nullptr)
	{
		MainMenu->AddToViewport();
	}
}

/* Bind Character to Feather Destroy Event. Called in GameMode */
void AMainPlayerController::BindFeatherCollectedEvent()
{
	/* Actors must be instantiated before GameMode Super::BeginPlay otherwise BasicPlayerController cannot find them */
	FeatherClassRef = AFeather::StaticClass();

	/* Populate TArray FoundFeathersInLevel with elements of AFeather Instantiated in the World */ 
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), FeatherClassRef, FoundFeathersInLevel);

	/* Number of Total Feathers spawned into the Scene by a SpawnFeather Component */ 
	TotalFeathers = FoundFeathersInLevel.Num();
	// UDebug::Print(WARNING, "Total Feathers in Level: " + FString::FromInt(TotalFeathers));

	/* Array loop for TArray of Actor Class Instances */
	for (AActor*& Val: FoundFeathersInLevel)
	{
		/* Getting a reference to the specific coin in the TArray */
		AFeather* Feather = Cast<AFeather>(Val);

		/* IsValidLowLevel() is a Null reference check but allows Unreal elements to be passed */ 
		if (!Feather->IsValidLowLevel()) { UDebug::Print(ERROR, "AMainPlayerController::BindFeatherCollectedEvent() Cast<AFeather>(Val) is invalid."); return; }
        
		/* Binding to the AFeather Event Dispatch OnFeatherCollected so we can pass args to our local method
		 * BindFeatherCollectedEvent when Dynamically called */
		Feather->OnFeatherCollected.AddDynamic(this,&AMainPlayerController::ObserveFeatherCollected);
	}
}

void AMainPlayerController::ObserveFeatherCollected(bool bFeatherCollected)
{
	if (!bFeatherCollected) { UDebug::Print(ERROR, "AMainPlayerController::BindFeatherCollectedEvent isn't receiving the correct bool Arg."); return; }

	CollectedFeathers++;

	if (GameModeRef && CollectedFeathers == TotalFeathers)
	{
		// UDebug::Print(WARNING, "All Feathers Found");
		AMainCharacter* Main = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		Main->MaximumJumps = 2;

		GameModeRef->DisplayDoubleJumpWidget();
	}
	else if (GameModeRef && CollectedFeathers != TotalFeathers)
	{
		GameModeRef->DisplayCollectedFeathersWidget();
	}
}
