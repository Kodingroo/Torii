// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ToriiGameMode.h"

#include "Blueprint/UserWidget.h"
#include "Characters/MainCharacter.h"
#include "Characters/MainPlayerController.h"
#include "Core/Debug.h"
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
	const UWorld* World = GEngine->GameViewport->GetWorld();

	AMainPlayerController* PlayerController = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(World, 0));
	if (PlayerController)
	{
		PlayerController->OpenMenu();
	}
}

void AToriiGameMode::SetupPlayer()
{
	DefaultPawnClass = AMainCharacter::StaticClass();
	HUDClass = ATorii_HUD::StaticClass();
	// PlayerControllerClass = AMainPlayerController::StaticClass();
	// GameStateClass = AToriiGameMode::StaticClass();
	/* Game Instance and Game Mode must be assigned in the Editor */ 
}

