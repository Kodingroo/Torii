// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TORII_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OpenMenu();

	void GainedDash();

	void CollectedFeathers();

	virtual void SetupInputComponent() override;
	
private:
	UPROPERTY(EditAnywhere, Category="Widgets")
	TSubclassOf<UUserWidget> MainMenuWidget;
	
};
