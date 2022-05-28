// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MainPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Core/Debug.h"


void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
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
	UDebug::Print(WARNING, "Open MEnu in Controller ");

	UUserWidget* MainMenu = CreateWidget(this, MainMenuWidget);
	if (MainMenu != nullptr)
	{
		MainMenu->AddToViewport();
	}
}

void AMainPlayerController::GainedDash()
{
}

void AMainPlayerController::CollectedFeathers()
{
}

