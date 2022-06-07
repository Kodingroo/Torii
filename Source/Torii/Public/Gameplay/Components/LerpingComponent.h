/* TORII: Pixel 2D Platformer - GAMEDEV.TV Game Jam Project - Copyright Isaac Hayward 2022 */

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

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* Variables from Sin Function */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sin")
	bool bShouldLoop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sin");
	float Amplitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sin");
	float Period;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sin");
	float PhaseShift;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sin");
	float VerticalShift;
	
	/* Variables for True Lerp */ 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lerp")
	float LerpDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lerp")
	float WaitTime;
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category="Actor Reference")
	AActor* Parent;
	
	/* Variables from Sin Function */
	float RunningTime;
	float BaseZLocation;
	
	/* Variables for True Lerp */ 
	float TimeElapsed = 0;
	FVector StartLocation;
	FVector TargetLocation;
};
