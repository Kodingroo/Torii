/* TORII: Pixel 2D Platformer - GAMEDEV.TV Game Jam Project - Copyright Isaac Hayward 2022 */

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
