/* TORII: Pixel 2D Platformer - GAMEDEV.TV Game Jam Project - Copyright Isaac Hayward 2022 */

#include "Gameplay/LerpingPlatform.h"


// Sets default values
ALerpingPlatform::ALerpingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PaperSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SpriteComponent"));
	PaperSprite->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void ALerpingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	AttachLerpingComponent();
}

// Called every frame
void ALerpingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ALerpingPlatform::AttachLerpingComponent()
{
	LerpingComponent = NewObject<ULerpingComponent>(this, ULerpingComponent::StaticClass(), TEXT("Lerping Component"));

	if (LerpingComponent)
	{
		LerpingComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		LerpingComponent->RegisterComponentWithWorld(GetWorld());
		LerpingComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}
}