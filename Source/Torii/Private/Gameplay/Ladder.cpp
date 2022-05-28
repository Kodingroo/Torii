// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Ladder.h"

#include "PaperSpriteComponent.h"
#include "Characters/MainCharacter.h"
#include "Core/Debug.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALadder::ALadder() :
	LadderHeight()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionVolume"));
	CollisionVolume->SetRelativeScale3D(FVector(16.f, 0.f, 48.f));
	RootComponent = CollisionVolume;

}

// Called when the game starts or when spawned
void ALadder::BeginPlay()
{
	Super::BeginPlay();

	CollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &ALadder::OnOverlapBegin);
	CollisionVolume->OnComponentEndOverlap.AddDynamic(this, &ALadder::OnOverlapEnd);
	
	Player = Cast<AMainCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

// Called every frame
void ALadder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALadder::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Player->OnLadder = true;
}

void ALadder::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Player->OnLadder = false;
	Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}


