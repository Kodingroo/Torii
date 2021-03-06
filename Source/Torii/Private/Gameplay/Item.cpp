/* TORII: Pixel 2D Platformer - GAMEDEV.TV Game Jam Project - Copyright Isaac Hayward 2022 */

#include "Gameplay/Item.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Core/Debug.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = false;

	/* Setting collision for use in child objects Pickup etc.*/ 
	CollisionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionVolume"));
	RootComponent = CollisionVolume;

	/* Setting up Mesh Component to be assigned by children */
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	CollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnOverlapBegin);
}

void AItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

