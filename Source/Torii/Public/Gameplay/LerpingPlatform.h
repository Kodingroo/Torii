/* TORII: Pixel 2D Platformer - GAMEDEV.TV Game Jam Project - Copyright Isaac Hayward 2022 */

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteComponent.h"
#include "Components/LerpingComponent.h"
#include "GameFramework/Actor.h"
#include "LerpingPlatform.generated.h"

UCLASS()
class TORII_API ALerpingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	ALerpingPlatform();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sprite")
	UPaperSpriteComponent* PaperSprite;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sprite")
	ULerpingComponent* LerpingComponent;
	
	UFUNCTION(BlueprintCallable, Category="Components")
	void AttachLerpingComponent();
	
protected:
	virtual void BeginPlay() override;
};
