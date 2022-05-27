// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Feather.h"

#include "Kismet/GameplayStatics.h"

#include "Characters/MainCharacter.h"
#include "Core/Debug.h"

UStaticMesh* CoinMesh;

AFeather::AFeather() :
	CoinSize(FVector(3.f))
{
	PrimaryActorTick.bCanEverTick = true;

	CoinMesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Props/Coin/Meshes/SM_Pickup_Coin.SM_Pickup_Coin'")).Object;
	Mesh->SetStaticMesh(CoinMesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/* Rotation inherited from Pickup */
	SetActorRelativeScale3D(CoinSize);

	/* Load Sound Cue */
	static ConstructorHelpers::FObjectFinder<USoundCue> CoinSoundCueObject(TEXT("SoundCue'/Game/SoundAssets/SC_Coin.SC_Coin'"));
	if (CoinSoundCueObject.Succeeded())
	{
		CoinSoundCue = CoinSoundCueObject.Object;
	}
}

void AFeather::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
						   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	const AK_BasicCharacter* Player = Cast<AK_BasicCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	
	if (OtherActor != nullptr && OtherActor != this && OtherActor == Player)
	{
		/* Broadcasting the Event through the Assigned Dispatch in the header to any Observers interested in Binding to the Event */
		OnCoinCollected.Broadcast(true);

		/* Sound Effects */ 
		UGameplayStatics::PlaySound2D(GetWorld(), CoinSoundCue);
		
		Destroy();
	}
}
