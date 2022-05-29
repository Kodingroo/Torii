// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "Components/LerpingComponent.h"
#include "GameFramework/Actor.h"
#include "LerpingPlatform.generated.h"

UCLASS()
class TORII_API ALerpingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALerpingPlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sprite")
	UPaperSpriteComponent* PaperSprite;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sprite")
	ULerpingComponent* LerpingComponent;

	UFUNCTION(BlueprintCallable, Category="Components")
	void AttachLerpingComponent();
};
