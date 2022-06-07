/* TORII: Pixel 2D Platformer - GAMEDEV.TV Game Jam Project - Copyright Isaac Hayward 2022 */

#include "Gameplay/InteractiveLamp.h"

#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "Gameplay/Ladder.h"
#include "Gameplay/Components/InteractionComponent.h"
#include "Kismet/GameplayStatics.h"

AInteractiveLamp::AInteractiveLamp()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LampFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("SpriteComponent"));

	InteractComp = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractComp"));

}

void AInteractiveLamp::BeginPlay()
{
	Super::BeginPlay();

	CollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AInteractiveLamp::OnOverlapBegin);
	CollisionVolume->OnComponentEndOverlap.AddDynamic(this, &AInteractiveLamp::OnOverlapEnd);
	
	Player = Cast<AMainCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void AInteractiveLamp::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Player->OverlapLamp = true;
	Player->FoundNewInteractable(InteractComp);
}

void AInteractiveLamp::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Player->OverlapLamp = false;
}
