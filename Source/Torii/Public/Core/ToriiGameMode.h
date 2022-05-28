// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ToriiGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TORII_API AToriiGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AToriiGameMode();

	virtual void StartPlay() override;

	UFUNCTION()
	void OpenMenu();
	
private:
	void SetupPlayer();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category="Widgets")
	TSubclassOf<UUserWidget> MainMenuWidget;
	UUserWidget* MMW;
};
