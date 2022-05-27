// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Item.h"
#include "PickUp.generated.h"

UCLASS()
class TORII_API APickUp : public AItem
{
	GENERATED_BODY()
	
public:
	APickUp();

	virtual void Tick(float DeltaTime) override;
	
};
