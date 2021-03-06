/* TORII: Pixel 2D Platformer - GAMEDEV.TV Game Jam Project - Copyright Isaac Hayward 2022 */

#include "Gameplay/Feather.h"

#include "PaperFlipbookComponent.h"
#include "PaperSpriteComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Characters/MainCharacter.h"
#include "Core/Debug.h"

UStaticMesh* FeatherMesh;

AFeather::AFeather() :
	FeatherSize(FVector(3.f))
{
	PrimaryActorTick.bCanEverTick = true;
	
	/* Rotation inherited from Pickup */

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorRelativeScale3D(FeatherSize);
	FB_Feather = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("FeatherFlipbook"));
}

void AFeather::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
						   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	const AMainCharacter* Player = Cast<AMainCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	
	if (OtherActor != nullptr && OtherActor != this && OtherActor == Player)
	{
		/* Broadcasting the Event through the Assigned Dispatch in the header to any Observers interested in Binding to the Event */
		OnFeatherCollected.Broadcast(true);

		/* Sound Effects */ 
		UGameplayStatics::PlaySound2D(GetWorld(), FeatherSoundCue);
		
		Destroy();
	}
}
