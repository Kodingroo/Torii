// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "PS_Platform.generated.h"

/**
 * 
 */
UCLASS()
class TORII_API APS_Platform : public APaperSpriteActor
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
};
