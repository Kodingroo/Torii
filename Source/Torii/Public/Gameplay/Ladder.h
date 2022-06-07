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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Item | Ladder")
	float LadderHeight;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Item | Collision")
	UBoxComponent* CollisionVolume;

	UFUNCTION(BlueprintCallable, Category="Overlap Events")
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable, Category="Overlap Events")
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	AMainCharacter* Player;
};
