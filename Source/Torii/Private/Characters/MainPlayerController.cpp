// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MainPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Core/Debug.h"


void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMainPlayerController::OpenMenu()
{
	UUserWidget* MainMenu = CreateWidget(this, MainMenuWidget);
	if (MainMenu != nullptr)
	{
		UDebug::Print(WARNING, "MainMenu loaded");
		MainMenu->AddToViewport();
	}
}

void AMainPlayerController::GainedDash()
{
}

void AMainPlayerController::CollectedFeathers()
{
}
