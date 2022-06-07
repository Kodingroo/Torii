/* TORII: Pixel 2D Platformer - GAMEDEV.TV Game Jam Project - Copyright Isaac Hayward 2022 */

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Item.generated.h"

class USphereComponent;
UCLASS()
class TORII_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Item | Collision")
	UBoxComponent* CollisionVolume;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Item | Mesh")
	UStaticMeshComponent* Mesh;
	
	UFUNCTION(BlueprintCallable, Category="Overlap Events")
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable, Category="Overlap Events")
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	virtual void BeginPlay() override;
};
