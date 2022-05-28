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

private:
	void SetupPlayer();
	
};
