/* TORII: Pixel 2D Platformer - GAMEDEV.TV Game Jam Project - Copyright Isaac Hayward 2022 */

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "Characters/MainCharacter.h"
#include "Gameplay/Item.h"
#include "Ladder.generated.h"

UCLASS()
class TORII_API ALadder : public AActor
{
	GENERATED_BODY()
	
public:	
	ALadder();

	float LadderHeight;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	AMainCharacter* Player;

public:	
	/** Overlapping Box */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Item | Collision")
	UBoxComponent* CollisionVolume;

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
