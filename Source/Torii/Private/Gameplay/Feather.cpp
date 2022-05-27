// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Feather.h"

#include "PaperSpriteComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Characters/MainCharacter.h"
#include "Core/Debug.h"

UStaticMesh* FeatherMesh;

AFeather::AFeather() :
	FeatherSize(FVector(3.f))
{
	PrimaryActorTick.bCanEverTick = true;

	// FeatherMesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Props/Feather/Meshes/SM_Pickup_Feather.SM_Pickup_Feather'")).Object;
	// Mesh->SetStaticMesh(FeatherMesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/* Rotation inherited from Pickup */
	SetActorRelativeScale3D(FeatherSize);

	/* Load Sound Cue */
	static ConstructorHelpers::FObjectFinder<USoundCue> FeatherSoundCueObject(TEXT("SoundCue'/Game/SoundAssets/SC_Feather.SC_Feather'"));
	if (FeatherSoundCueObject.Succeeded())
	{
		FeatherSoundCue = FeatherSoundCueObject.Object;
	}
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
