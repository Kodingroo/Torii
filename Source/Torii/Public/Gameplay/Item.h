// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;
UCLASS()
class TORII_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	/** Overlapping Sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Item | Collision")
	USphereComponent* CollisionVolume;

	/** Base Mesh Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Item | Mesh")
	UStaticMeshComponent* Mesh;
	
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
