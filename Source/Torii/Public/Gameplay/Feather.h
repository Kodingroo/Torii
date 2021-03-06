/* TORII: Pixel 2D Platformer - GAMEDEV.TV Game Jam Project - Copyright Isaac Hayward 2022 */

#pragma once

#include "CoreMinimal.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "Gameplay/PickUp.h"
#include "Sound/SoundCue.h"
#include "Feather.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFeatherCollected, bool, bFeatherCollected);

UCLASS()
class TORII_API AFeather : public APickUp
{
	GENERATED_BODY()
	
public:
	AFeather();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations)
	UPaperFlipbookComponent* FB_Feather;

	/* Declaring a Delegate/ Event Dispatcher and setting it up as a Property people can interact with */
	UPROPERTY(BlueprintAssignable, Category="EventDispatches")
	FOnFeatherCollected OnFeatherCollected;

	/* Determine the size of the Feather */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Feather | FeatherProperties")
	FVector FeatherSize;

	/* Sound Effect */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Audio")
	USoundCue* FeatherSoundCue;

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
