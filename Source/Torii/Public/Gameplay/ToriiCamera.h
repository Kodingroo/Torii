// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ToriiCamera.generated.h"

UCLASS()
class TORII_API AToriiCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	AToriiCamera();

protected:
	virtual void BeginPlay() override;


};
