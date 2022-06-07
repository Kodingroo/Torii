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
	// Sets default values for this actor's properties
	AInteractiveLamp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations)
	UPaperFlipbookComponent* LampFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player")
	AMainCharacter* Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InteractComp")
	UInteractionComponent* InteractComp;
};
