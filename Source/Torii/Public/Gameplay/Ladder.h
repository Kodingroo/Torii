// Fill out your copyright notice in the Description page of Project Settings.

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
	// Sets default values for this actor's properties
	ALadder();

	float LadderHeight;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	AMainCharacter* Player;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Overlapping Box */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Item | Collision")
	UBoxComponent* CollisionVolume;

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
