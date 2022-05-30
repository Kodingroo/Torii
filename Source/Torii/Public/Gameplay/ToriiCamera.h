/* TORII: Pixel 2D Platformer - GAMEDEV.TV Game Jam Project - Copyright Isaac Hayward 2022 */

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
