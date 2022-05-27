// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "LerpingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TORII_API ULerpingComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	ULerpingComponent();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category="Actor Reference")
	AActor* Parent = nullptr;
	
	/* Variables from Sin Function */
	float RunningTime;
	float BaseZLocation;
	
	/* Variables for True Lerp */ 
	float TimeElapsed = 0;
	FVector StartLocation;
	FVector TargetLocation;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* Variables from Sin Function */
	UPROPERTY(EditAnywhere, Category="Sin")
	bool bShouldLoop;
	UPROPERTY(EditAnywhere, Category="Sin");
	float Amplitude;
	UPROPERTY(EditAnywhere, Category="Sin");
	float Period;
	UPROPERTY(EditAnywhere, Category="Sin");
	float PhaseShift;
	UPROPERTY(EditAnywhere, Category="Sin");
	float VerticalShift;
	
	/* Variables for True Lerp */ 
	UPROPERTY(EditAnywhere, Category="Lerp")
	float LerpDuration;
	UPROPERTY(EditAnywhere, Category="Lerp")
	float WaitTime;
};
