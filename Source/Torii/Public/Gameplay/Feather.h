// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/PickUp.h"
#include "Sound/SoundCue.h"
#include "Feather.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFeatherCollected, bool, bFeatherCollected);

UCLASS()
class UE4_KODINGROO_API AFeather : public APickUp
{
	GENERATED_BODY()
	
public:
	AFeather();

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
