/* TORII: Pixel 2D Platformer - GAMEDEV.TV Game Jam Project - Copyright Isaac Hayward 2022 */

#include "Core/ToriiGameMode.h"

#include "Blueprint/UserWidget.h"
#include "Characters/MainCharacter.h"
#include "Characters/MainPlayerController.h"
#include "Core/Debug.h"
#include "Core/ToriiGameState.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Torii_HUD.h"

AToriiGameMode::AToriiGameMode()
{
	SetupPlayer();
	
}

void AToriiGameMode::StartPlay()
{

	Super::StartPlay();
	
	InitFeatherCollectedEvent();
}

void AToriiGameMode::SetupPlayer()
{
	DefaultPawnClass = AMainCharacter::StaticClass();
	HUDClass = ATorii_HUD::StaticClass();
	PlayerControllerClass = AMainPlayerController::StaticClass();
	GameStateClass = AToriiGameState::StaticClass();
	/* Game Instance and Game Mode must be assigned in the Editor */ 
}

void AToriiGameMode::BeginPlay()
{
	Super::BeginPlay();

	OpenMenu();

	MainPlayerController = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(GEngine->GameViewport->GetWorld(), 0));

	/* Must create reference to Widgets in BeginPlay to avoid unexpected behavior */ 
	CFW = Cast<UUserWidget>(CreateWidget(GetWorld(), CollectFeatherWidget));
	DJW = Cast<UUserWidget>(CreateWidget(GetWorld(), DoubeJumpWidget));
}

void AToriiGameMode::OpenMenu()
{
	if (MainMenuWidget)
	{
		MMW = Cast<UUserWidget>(CreateWidget(GetWorld(), MainMenuWidget));
		if (MMW)
		{
			MMW->AddToViewport();

			if (MainPlayerController)
			{
				MainPlayerController->bShowMouseCursor = true;
			}
		}
	}
}


void AToriiGameMode::InitFeatherCollectedEvent() const
{
	if (MainPlayerController)
	{
		// UDebug::Print(WARNING, "You collected a feather");
		MainPlayerController->BindFeatherCollectedEvent();
	}    
}


void AToriiGameMode::DisplayCollectedFeathersWidget()
{
	if (!MainPlayerController)
	{
		return;
	}
	
	if (CFW && MainPlayerController->TotalFeathers != MainPlayerController->CollectedFeathers)
	{
		CFW->AddToViewport();
		MainPlayerController->bShowMouseCursor = true;
		// UDebug::Print(WARNING, "DisplayCollectedFeathersWidget");
	}
}

void AToriiGameMode::DisplayDoubleJumpWidget()
{
	if (!MainPlayerController)
	{
		return;
	}
	
	if (DJW && MainPlayerController->TotalFeathers == MainPlayerController->CollectedFeathers)
	{
		DJW->AddToViewport();
		CFW->RemoveFromParent();
		MainPlayerController->bShowMouseCursor = true;
	}
}
