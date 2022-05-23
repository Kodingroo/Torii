// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ToriiGameMode.h"
#include "Characters/MainCharacter.h"
#include "Characters/MainPlayerController.h"

AToriiGameMode::AToriiGameMode()
{
	DefaultPawnClass = AMainCharacter::StaticClass();
	PlayerControllerClass = AMainPlayerController::StaticClass(); 
	
}

