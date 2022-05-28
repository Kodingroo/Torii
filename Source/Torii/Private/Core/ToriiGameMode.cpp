// Fill out your copyright notice in the Description page of Project Settings.


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
	
	/* Display Main Menu when first loading only */ 
	// const UWorld* World = GEngine->GameViewport->GetWorld();

	/* Displaying Main Menu in Blueprints */ 
	// UDebug::Print(WARNING, "Game Mode Start Play");
	// AMainPlayerController* PlayerController = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(World, 0));
	// if (PlayerController)
	// {
	// 	UDebug::Print(WARNING, "Game Mode Controller call");
	// 	PlayerController->OpenMenu();
	// }
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
}

void AToriiGameMode::OpenMenu()
{
	if (MainMenuWidget)
	{
		MMW = Cast<UUserWidget>(CreateWidget(GetWorld(), MainMenuWidget));
		if (MMW)
		{
			MMW->AddToViewport();
			AMainPlayerController* PC = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)); 
			PC->bShowMouseCursor = true;
		}
	}
}

