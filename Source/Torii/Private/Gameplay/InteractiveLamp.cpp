/* TORII: Pixel 2D Platformer - GAMEDEV.TV Game Jam Project - Copyright Isaac Hayward 2022 */

#include "Gameplay/InteractiveLamp.h"

#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "Gameplay/Ladder.h"
#include "Gameplay/Components/InteractionComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AInteractiveLamp::AInteractiveLamp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LampFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("SpriteComponent"));

	InteractComp = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractComp"));

}

// Called when the game starts or when spawned
void AInteractiveLamp::BeginPlay()
{
	Super::BeginPlay();

	CollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AInteractiveLamp::OnOverlapBegin);
	CollisionVolume->OnComponentEndOverlap.AddDynamic(this, &AInteractiveLamp::OnOverlapEnd);
	
	Player = Cast<AMainCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

// Called every frame
void AInteractiveLamp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
