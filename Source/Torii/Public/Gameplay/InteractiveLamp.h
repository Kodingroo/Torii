/* TORII: Pixel 2D Platformer - GAMEDEV.TV Game Jam Project - Copyright Isaac Hayward 2022 */

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "PaperFlipbookComponent.h"
#include "Components/InteractionComponent.h"
#include "InteractiveLamp.generated.h"

UCLASS()
class TORII_API AInteractiveLamp : public AItem
{
	GENERATED_BODY()
	
public:	
	AInteractiveLamp();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animations")
	UPaperFlipbookComponent* LampFlipbook;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interact Component")
	UInteractionComponent* InteractComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="References | Player")
	AMainCharacter* Player;
	
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex);
protected:
	virtual void BeginPlay() override;
};
